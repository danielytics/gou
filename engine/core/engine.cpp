
#include "engine.hpp"
#include "graphics/graphics.hpp"

#include <SDL.h>

#include <entt/core/type_info.hpp>
namespace gou::api::detail {
    #include <gou/type_info.hpp>
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

gou::api::Renderer& core::Engine::renderer () const
{
    return *m_renderer;
}

entt::registry& core::Engine::registry(gou::api::Registry which)
{
    switch (which) {
    case gou::api::Registry::Runtime:
        return m_registry;
    case gou::api::Registry::Background:
        return m_background_registry;
    case gou::api::Registry::Prototype:
        return m_prototype_registry;
    };
}

entt::organizer& core::Engine::organizer(gou::api::SystemStage type)
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

const std::vector<gou::api::definitions::Component>& core::Engine::getRegisteredComponents ()
{
    return m_component_definitions;
}

gou::resources::Handle core::Engine::findResource (entt::hashed_string::hash_type name)
{
    return {};
}

gou::resources::Signal core::Engine::findSignal (entt::hashed_string::hash_type)
{
    return {};
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
        switch (event.type) {
            case "engine/exit"_event:
                return false;
            case "engine/set-system-status/running"_event:
                m_system_status = SystemStatus::Running;
                break;
            case "engine/set-system-status/stopped"_event:
                m_system_status = SystemStatus::Stopped;
                break;
            case "scene/registry/runtime->background"_event:
                copyRegistry(m_registry, m_background_registry);
                break;
            case "scene/registry/background->runtime"_event:
                copyRegistry(m_background_registry, m_registry);
                break;
            case "scene/registry/clear-background"_event:
                m_background_registry.clear();
                break;
            case "scene/registry/clear-runtime"_event:
                m_registry.clear();
                break;
            default:
                break;
        };
    }

    // Run the before-frame hook for each module, updating the current time
    callModuleHook<CM::BEFORE_FRAME>(current_time, delta, frame_count);

    if (m_system_status == SystemStatus::Running) {
        // Execute the taskflow graph if systems are running
        m_executor.run(m_coordinator);
    } else {
        // If systems are stopped, only pump events
        pumpEvents();
    }

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

void core::Engine::copyRegistry (entt::registry& from, entt::registry& to)
{
    from.visit([&from, &to](const auto info) {
        std::as_const(from).storage(info)->copy_to(to);
    });
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
