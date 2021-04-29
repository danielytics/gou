
#include "engine.hpp"
#include "graphics/graphics.hpp"
#include "physics/physics.hpp"

#include <SDL.h>

#include <entt/core/type_info.hpp>
namespace gou::api::detail {
    #include <gou/type_info.hpp>
}

namespace gou {
    void register_components (gou::api::Engine*);
}

using CM = gou::api::Module::CallbackMasks;


// Taskflow workers = number of hardware threads - 1, unless there is only one hardware thread
int get_num_workers () {
    auto max_workers = std::thread::hardware_concurrency();
    return max_workers > 1 ? max_workers - 1 : max_workers;
}

int get_global_event_pool_size () {
    const std::uint32_t pool_size = entt::monostate<"memory/events/pool-size"_hs>();
    return pool_size * get_num_workers();
}

core::Engine::Engine () :
    m_scene_manager(*this),
    m_executor(get_num_workers()),
    m_event_pool(get_global_event_pool_size())
{
    // Manage Named entities
    m_registry.on_construct<components::Named>().connect<&core::Engine::onAddNamedEntity>(this);
    m_registry.on_destroy<components::Named>().connect<&core::Engine::onRemoveNamedEntity>(this);
    // Manage prototype entities
    m_prototype_registry.on_construct<core::EntityPrototypeID>().connect<&core::Engine::onAddPrototypeEntity>(this);
    m_prototype_registry.on_destroy<core::EntityPrototypeID>().connect<&core::Engine::onRemovePrototypeEntity>(this);
}

core::Engine::~Engine ()
{

}

gou::api::detail::type_context* core::Engine::type_context() const
{
    return gou::api::detail::type_context::instance();
}

void core::Engine::registerModule (std::uint32_t flags, gou::api::Module* mod)
{
    using CM = gou::api::Module::CallbackMasks;

    // All modules have the before-frame hook registered
    addModuleHook(CM::BEFORE_FRAME, mod);

    // Register optional hooks
    for (auto hook : {CM::AFTER_FRAME, CM::LOAD_SCENE, CM::UNLOAD_SCENE, CM::BEFORE_UPDATE, CM::PREPARE_RENDER, CM::BEFORE_RENDER, CM::AFTER_RENDER}) {
        if (flags & helpers::enum_value(hook)) {
            addModuleHook(hook, mod);
        }
    }
}

gou::api::Renderer& core::Engine::renderer () const
{
    return *m_renderer;
}

entt::registry& core::Engine::registry()
{
    return m_registry;
}

entt::organizer& core::Engine::organizer(std::uint32_t type)
{
    return m_organizers[type];
}

entt::entity core::Engine::findEntity (entt::hashed_string name) const
{
    auto it = m_named_entities.find(name);
    if (it != m_named_entities.end()) {
        return it->second.entity;
    }
    return entt::null;
}

const std::string& core::Engine::findEntityName (const components::Named& named) const
{
    auto it = m_named_entities.find(named.name);
    if (it != m_named_entities.end()) {
        return it->second.name;
    }
    return m_empty_string;
}

void core::Engine::registerLoader(entt::hashed_string name, gou::api::Engine::LoaderFn loader_fn)
{
    m_component_loaders[name] = loader_fn;
}

gou::resources::Handle core::Engine::findResource (entt::hashed_string::hash_type name)
{
    return {};
}

void* core::Engine::allocModule (std::size_t bytes) {
    return reinterpret_cast<void*>(new std::byte[bytes]);
}
void core::Engine::deallocModule (void* ptr) {
    delete [] reinterpret_cast<std::byte*>(ptr);
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

void core::Engine::loadComponent (core::Engine::EntityLoadType loadType, entt::hashed_string component, entt::entity entity, const void* table)
{
    auto it = m_component_loaders.find(component);
    if (it != m_component_loaders.end()) {
        const auto& loader = it->second;
        auto& registry = (loadType == core::Engine::EntityLoadType::LoadToScene) ? m_registry : m_prototype_registry;
        loader(this, registry, table, entity);
    } else {
        spdlog::warn("Tried to load non-existent component: {}", component.data());
    }
}

entt::registry& core::Engine::prototypeRegistry ()
{
    return m_prototype_registry;
}


void core::Engine::createTaskGraph () {
    // Setup Systems by creating a Taskflow graph for each stage
    using Stage = gou::api::SystemStage;
    auto registry = &m_registry;
    spp::sparse_hash_map<Stage, tf::Taskflow*> taskflows;
    for (auto type : {Stage::GameLogic, Stage::Update}) {
        auto it = m_organizers.find(helpers::enum_value(type));
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
    return imgui_ctx;
}

void core::Engine::setupGame ()
{
    // Initialise subsystems
    m_physics_context = physics::init(*this);
    // Create task graph
    createTaskGraph();
    // Load game data
    setupInitialScene();
    // Make events generated during module loading available during first frame
    pumpEvents();
}

void core::Engine::handleInput ()
{
    /**
     * Gather input from input devices: keyboard, mouse, gamepad, joystick
     * Input is mapped to events and those events are emitted for systems to process.
     * handleInput doesn't use the engines normal emit() API, instead it adds the events
     * directly to the m_event_pool global event pool. This way, input-generated events
     * are immediately available, without a frame-delay. We can do this, because handleInput
     * is guaranteed to be called serially, before the taskflow graph is executed, so we can
     * guarantee 
     */

    SDL_Event event;
    m_input_events.clear();
    // Gather and dispatch input
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
                internalEmplaceEvent("engine/exit"_event);
                break;
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    entt::monostate<"graphics/resolution/width"_hs>{} = int(event.window.data1);
                    entt::monostate<"graphics/resolution/height"_hs>{} = int(event.window.data2);
                    graphics::windowChanged(m_renderer);
                    break;
                default:
                    break;
                };
                break;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    internalEmplaceEvent("engine/exit"_event);
                } else {
                    // handleInput(engine, input_mapping, InputKeys::KeyType::KeyboardButton, event.key.keysym.scancode, [&event]() -> float {
                    //     return event.key.state == SDL_PRESSED ? 1.0f : 0.0f;
                    // });
                }
                break;
            }
            case SDL_CONTROLLERDEVICEADDED:
                // TODO: Handle multiple gamepads
                m_game_controller = SDL_GameControllerOpen(event.cdevice.which);
                if (m_game_controller == 0) {
                    spdlog::error("Could not open gamepad {}: {}", event.cdevice.which, SDL_GetError());
                } else {
                    spdlog::info("Gamepad detected {}", SDL_GameControllerName(m_game_controller));
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                SDL_GameControllerClose(m_game_controller);
                break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
            {
                // handleInput(engine, input_mapping, InputKeys::KeyType::ControllerButton, event.cbutton.button, [&event]() -> float {
                //     return event.cbutton.state ? 1.0f : 0.0f;
                // });
                break;
            }
            case SDL_CONTROLLERAXISMOTION:
            {
                // handleInput(engine, input_mapping, InputKeys::KeyType::ControllerAxis, event.caxis.axis, [&event]() -> float {
                //     float value = float(event.caxis.value) * Constants::GamePadAxisScaleFactor;
                //     if (std::fabs(value) < 0.15f /* TODO: configurable deadzones */) {
                //         return 0;
                //     }
                //     return value;
                // });
                break;
            }
            default:
                break;
        };
        // Store events for render thread to access (used by Dear ImGui)
        m_input_events.push_back(event);
    }

    // Make sure that any events that were dispatched are visible to the engine
    refreshEventsIterator();
}

bool core::Engine::execute (Time current_time, DeltaTime delta, uint64_t frame_count)
{
    m_current_time_delta = delta;

    // Read input device states and dispatch events. Input events are emitted directly into the global pool, immediately readable "this frame" (no frame delay!)
    handleInput();

    // // Process previous frames events, looking for ones the core engine cares about
    // Yes, its a bit wasteful to loop them all like this, but they should be hot in cache so ¯\_(ツ)_/¯
    for (auto& event : helpers::const_iterate(events())) {
        if (event.type == "engine/exit"_event) {
            // No longer running, return false
            return false;
        }
    }

    // Run the before-frame hook for each module, updating the current time
    callModuleHook<CM::BEFORE_FRAME>(current_time, delta, frame_count);

    // Execute the taskflow graph
    m_executor.run(m_coordinator);

    // Run the after-frame hook for each module
    callModuleHook<CM::AFTER_FRAME>();

    /*
     * Hand over exclusive access to engine state to renderer.
     * Renderer will access the ECS registry to gather all components needed for rendering, accumulate
     * a render list and hand exclusive access back to the engine. The renderer wil then asynchronously
     * render from its locally owned render list.
     */

    // First, signal to the renderer that it has exclusive access to the engines state
    {
        std::scoped_lock<std::mutex> lock(m_graphics_sync->state_mutex);
        m_graphics_sync->owner = graphics::Sync::Owner::Renderer;
    }
    m_graphics_sync->sync_cv.notify_one();

    // Now wait for the renderer to relinquish exclusive access back to the engine
    std::unique_lock<std::mutex> lock(m_graphics_sync->state_mutex);
    m_graphics_sync->sync_cv.wait(lock, [this]{ return m_graphics_sync->owner == graphics::Sync::Owner::Engine; });

    /*
     * Engine has exclusive access again.
     */

    // Still running, return true.
    return true;
}

void core::Engine::reset ()
{
    // Unload the current scene
    callModuleHook<CM::UNLOAD_SCENE>();
    // Shut down graphics thread
    graphics::term(m_renderer);
    // Delete event pools
    for (auto pool : m_event_pools) {
        delete pool;
    }
    // Clear the registry
    m_registry = {};
    // Clear the prototype registry
    m_prototype_registry = {};
}

void core::Engine::setupInitialScene ()
{
    const std::string& scene_list_file = entt::monostate<"game/scene-list-file"_hs>();
    m_scene_manager.loadSceneList(scene_list_file);

    const std::string& start_scene = entt::monostate<"game/start-scene"_hs>();
    m_scene_manager.loadScene(entt::hashed_string{start_scene.c_str()});
}

void core::Engine::onAddNamedEntity (entt::registry& registry, entt::entity entity)
{
    const auto& named = registry.get<components::Named>(entity);
    m_named_entities[named.name] = {entity, named.name.data()};
}

void core::Engine::onRemoveNamedEntity (entt::registry& registry, entt::entity entity)
{
    const auto& named = registry.get<components::Named>(entity);
    m_named_entities.erase(named.name);
}
