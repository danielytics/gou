
#include "engine.hpp"
#include "graphics/graphics.hpp"

using CM = gou::api::Module::CallbackMasks;

#include <entt/core/type_info.hpp>
namespace gou::api::detail {
    #include <type_info.hpp>
}

// void core::Engine::setup(std::shared_ptr<spdlog::logger> logger) {
//     // physics::init(this);
//     // createSystems();
//     // loadModules(logger);
//     createTaskGraph();
//     // Make events generated during module loading available during first frame
//     pumpEvents();
// }

core::Engine::Engine ()
{
    for(auto i = 0.f; i < 3.f; ++i) {
        m_registry.emplace<components::Position>(m_registry.create(), i, i, 0.f);
    }
}

core::Engine::~Engine ()
{

}

gou::api::detail::type_context* core::Engine::type_context() {
    return gou::api::detail::type_context::instance();
}

void core::Engine::registerModule (std::uint32_t flags, gou::api::Module* mod)
{
    using CM = gou::api::Module::CallbackMasks;

    // All modules have the before-frame hook registered
    addModuleHook(CM::BEFORE_FRAME, mod);

    // Register optional hooks
    for (auto hook : {CM::AFTER_FRAME, CM::LOAD_SCENE, CM::UNLOAD_SCENE, CM::BEFORE_UPDATE, CM::BEFORE_RENDER, CM::AFTER_RENDER}) {
        if (flags & utilities::enum_value(hook)) {
            addModuleHook(hook, mod);
        }
    }
}

entt::registry& core::Engine::registry()
{
    return m_registry;
}

entt::organizer& core::Engine::organizer(std::uint32_t type)
{
    return m_organizers[type];
}

entt::entity core::Engine::findEntity (entt::hashed_string name)
{
    return entt::null;
}

entt::entity core::Engine::loadEntity (entt::hashed_string template_id)
{
    return entt::null;
}

void core::Engine::mergeEntity (entt::entity entity, entt::hashed_string template_id, bool overwrite_components)
{

}

void core::Engine::registerLoader(entt::hashed_string name, gou::api::Engine::LoaderFn loader_fn)
{

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
    for (auto type : {Stage::GameLogic, Stage::Update}) {
        auto it = m_organizers.find(utilities::enum_value(type));
        if (it != m_organizers.end()) {
            tf::Taskflow* taskflow = new tf::Taskflow();
            m_taskflows.push_back(taskflow);
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

    // Add engine-internal tasks to graph and coordinate flow into one graph    
    tf::Taskflow* game_logic_flow = m_taskflows[utilities::enum_value(Stage::GameLogic)];
    tf::Taskflow* updater_flow = m_taskflows[utilities::enum_value(Stage::Update)];
    game_logic_flow->name("Game Logic");
    updater_flow->name("State Update");
    tf::Task game_logic_tasks = m_coordinator.composed_of(*game_logic_flow).name("Systems");
    tf::Task updater_tasks = m_coordinator.composed_of(*updater_flow).name("Systems");
    // tf::Task physics_task_prepare = m_coordinator.emplace([this](){
    //     physics::prepare(m_registry);
    // }).name("Physics/prepare");
    //tf::Task physics_task_simulate = m_coordinator.emplace(physics::simulate).name("Physics/simulate");
    tf::Task before_update_task = m_coordinator.emplace([this](){
        callModuleHook<CM::BEFORE_UPDATE>();
    }).name("Hooks/before-update");
    tf::Task pump_events_task = m_coordinator.emplace([this](){
        pumpEvents(); // Copy current frames events for processing next frame
    }).name("Events/pump");

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
    game_logic_tasks.precede(before_update_task/*, physics_task_prepare*/);
    pump_events_task.succeed(before_update_task/*, physics_task_prepare*/);
    //physics_task_simulate.succeed(physics_task_prepare);
    updater_tasks.succeed(pump_events_task/*, physics_task_simulate*/);

#ifdef DEV_MODE
    const bool dev_mode = entt::monostate<"game/dev-mode"_hs>();
    if (dev_mode) {
        std::ofstream file("task_graph.dot", std::ios_base::out);
        m_coordinator.dump(file);
    }
#endif
}

void core::Engine::setupSystems (graphics::Sync* state_sync)
{
    m_state_sync = state_sync;
}

void core::Engine::execute (Time current_time, DeltaTime delta, uint64_t frame_count) {
    current_time_delta = delta;

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
        std::scoped_lock<std::mutex> lock(m_state_sync->state_mutex);
        m_state_sync->owner = graphics::Sync::Owner::Renderer;
    }
    m_state_sync->sync_cv.notify_one();

    // Now wait for the renderer to relinquish exclusive access back to the engine
    std::unique_lock<std::mutex> lock(m_state_sync->state_mutex);
    m_state_sync->sync_cv.wait(lock, [this]{ return m_state_sync->owner == graphics::Sync::Owner::Engine; });
}

void core::Engine::reset ()
{
    m_registry.view<components::Position>().each([](auto entity, auto &position) {
        std::cout << "(" << position.x << ", " << position.y << ", " << position.z << "): " << static_cast<int>(entt::to_integral(entity) + 16u) << "\n";
    });
    
    m_registry = {};
}

void core::Engine::pumpEvents () {
    // globalEventPool.reset();
    // // Copy thread local events into global pool and reset thread local pools
    // for (auto& eventPool : eventPools) {
    //     auto& pool = eventPool->pool;
    //     globalEventPool.copy<EventPool::PoolType>(pool);
    //     pool.reset();
    // }
    // // Create iterator for consumers
    // eventPoolIterator = {
    //     const_cast<EventPoolIterator::Type*>(globalEventPool.begin()),
    //     globalEventPool.count()
    // };
}
