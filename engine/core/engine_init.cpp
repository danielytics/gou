
#include "engine.hpp"
#include "physics/physics.hpp"
#include "graphics/graphics.hpp"

namespace gou {
    void register_components (gou::api::Engine*);
}


using CM = gou::api::Module::CallbackMasks;

void* core::Engine::allocModule (std::size_t bytes) {
    return reinterpret_cast<void*>(new std::byte[bytes]);
}

void core::Engine::deallocModule (void* ptr) {
    delete [] reinterpret_cast<std::byte*>(ptr);
}

void core::Engine::registerModule (std::uint32_t flags, gou::api::Module* mod)
{
    // All modules have the before-frame hook registered
    addModuleHook(CM::BEFORE_FRAME, mod);

    // Register optional hooks
    for (auto hook : {CM::AFTER_FRAME, CM::LOAD_SCENE, CM::UNLOAD_SCENE, CM::BEFORE_UPDATE, CM::PREPARE_RENDER, CM::BEFORE_RENDER, CM::AFTER_RENDER}) {
        if (flags & helpers::enum_value(hook)) {
            addModuleHook(hook, mod);
        }
    }
}

void core::Engine::registerComponent (gou::api::definitions::Component& component_def)
{
    m_component_loaders[component_def.id] = component_def.loader;
    m_component_definitions.push_back(component_def);
}

void core::Engine::addModuleHook (gou::api::Module::CallbackMasks hook, gou::api::Module* mod) {
    switch (hook) {
        case CM::BEFORE_FRAME:
            m_hooks_beforeFrame.push_back(mod);
            break;
        case CM::AFTER_FRAME:
            m_hooks_afterFrame.push_back(mod);
            break;
        case CM::BEFORE_UPDATE:
            m_hooks_beforeUpdate.push_back(mod);
            break;
        case CM::PREPARE_RENDER:
            m_hooks_prepareRender.push_back(mod);
            break;
        case CM::BEFORE_RENDER:
            m_hooks_beforeRender.push_back(mod);
            break;
        case CM::AFTER_RENDER:
            m_hooks_afterRender.push_back(mod);
            break;
        case CM::LOAD_SCENE:
            m_hooks_loadScene.push_back(mod);
            break;
        case CM::UNLOAD_SCENE:
            m_hooks_unloadScene.push_back(mod);
            break;
    };
}

void core::Engine::createTaskGraph () {
    // Setup Systems by creating a Taskflow graph for each stage
    using Stage = gou::api::SystemStage;
    auto registry = &m_registry;
    spp::sparse_hash_map<Stage, tf::Taskflow*> taskflows;
    for (auto type : {Stage::GameLogic, Stage::Update}) {
        auto it = m_organizers.find(type);
        if (it != m_organizers.end()) {
            tf::Taskflow* taskflow = new tf::Taskflow();
            taskflows[type] = taskflow;
            std::vector<std::pair<entt::organizer::vertex, tf::Task>> tasks;
            auto graph = it->second.graph();
            // First pass, prepare registry and create taskflow task
            for(auto&& node : graph) {
                spdlog::debug("Setting up system: {}", node.name());
                node.prepare(m_registry);
                auto callback = node.callback();
                auto userdata = node.data();
                auto name = node.name();
                tasks.push_back({
                    node,
                    taskflow->emplace([callback, userdata, registry, name](){
                        spdlog::trace("Running System: {}", name);
                        callback(userdata, *registry);
                    }).name(node.name())
                });
            }
            // Second pass, set parent-child relationship of tasks
            for (auto&& [node, task] : tasks) {
                for (auto index : node.children()) {
                    task.precede(tasks[index].second);
                }
            }
            it->second.clear();
        }
    }
    // Any new additions will never be added to the Taskflow graph, so no point in keeping the organizers around
    m_organizers.clear();

    /** Task graph:
     * 
     *                  GAME LOGIC [*]
     *                    /    \
     *       BEFORE UPDATE     PHYSICS PREPARE
     *          |               /       |
     *          PUMP EVENTS  <-+     PHYSICS SIMULATE
     *                    \             |
     *                     +-> UPDATE LOGIC [*]
     * 
     * [*] = GAME LOGIC & UPDATE LOGIC are modules of subtasks
     **/
    tf::Task physics_task_prepare = m_coordinator.emplace([this](){
        physics::prepare(m_physics_context, m_registry);
    }).name("Physics/prepare");
    tf::Task physics_task_simulate = m_coordinator.emplace([this](){
        physics::simulate(m_physics_context);
    }).name("Physics/simulate");
    tf::Task before_update_task = m_coordinator.emplace([this](){
        callModuleHook<CM::BEFORE_UPDATE>();
    }).name("Hooks/before-update");
    tf::Task pump_events_task = m_coordinator.emplace([this](){
        pumpEvents(); // Copy current frames events for processing next frame
    }).name("Events/pump");
    pump_events_task.succeed(before_update_task, physics_task_prepare);
    physics_task_simulate.succeed(physics_task_prepare);
    
    // Add engine-internal tasks to graph and coordinate flow into one graph
    if (tf::Taskflow* game_logic_flow = helpers::find_or(taskflows, Stage::GameLogic, nullptr)) {
        game_logic_flow->name("Game Logic");
        tf::Task game_logic_tasks = m_coordinator.composed_of(*game_logic_flow).name("Systems");
        game_logic_tasks.precede(before_update_task, physics_task_prepare);
    }
    if (tf::Taskflow* updater_flow = helpers::find_or(taskflows, Stage::Update, nullptr)) {
        updater_flow->name("State Update");
        tf::Task updater_tasks = m_coordinator.composed_of(*updater_flow).name("Systems");
        updater_tasks.succeed(pump_events_task, physics_task_simulate);
    }

#ifdef DEBUG_BUILD
    const bool dev_mode = entt::monostate<"game/dev-mode"_hs>();
    if (dev_mode) {
        std::ofstream file("task_graph.dot", std::ios_base::out);
        m_coordinator.dump(file);
    }
#endif
}

ImGuiContext* core::Engine::init ()
{
    // Setup renderer
    ImGuiContext* imgui_ctx;
    m_renderer = graphics::init(*this, m_graphics_sync, imgui_ctx);
    // Register core components
    gou::register_components(this);
    // Set system status
    m_system_status = SystemStatus::Stopped;

    return imgui_ctx;
}

void core::Engine::setupGame ()
{
    // Initialise subsystems
    m_physics_context = physics::init(*this);
    // Create task graph
    createTaskGraph();
    // Clear component definition data, only available during module loading
    m_component_definitions.clear();
    // Load game data
    setupInitialScene();
    // Make events generated during module loading available during first frame
    pumpEvents();
}

void core::Engine::setupInitialScene ()
{
    const std::string& scene_list_file = entt::monostate<"game/scene-list-file"_hs>();
    m_scene_manager.loadSceneList(scene_list_file);

    const std::string& start_scene = entt::monostate<"game/start-scene"_hs>();
    m_scene_manager.loadScene(entt::hashed_string{start_scene.c_str()});
}
