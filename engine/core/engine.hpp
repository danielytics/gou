#pragma once

#include "gou_engine.hpp"
#include <gou/api.hpp>
#include "world/scenes.hpp"

#include <SDL.h>

#include <taskflow/taskflow.hpp>

#include "entt_polystorage.hpp"


namespace graphics {
    struct Sync;
}
namespace physics {
    struct Context;
}

 struct ImGuiContext;

namespace core {

    // Component used by the prototypes registry to identify the prototype entity
    struct EntityPrototypeID {
        entt::hashed_string::hash_type id;
    };

    /**
     * Engine is the API through which functionality is registered with the engine.
     * This is responsible for accessing the ECS registry, registering up ECS system tasks, registering components and providing "core" services.
     * It is the API through which modules interact with the engine during setup, but is not typically used during gameplay runtime.
     */
    class Engine : public gou::api::Engine {
    public:
        Engine();
        virtual ~Engine();

        using EventPool = memory::StackPool<gou::events::Event, memory::AlignCacheLine>;

        // Implement API interface
        gou::api::detail::type_context* type_context() const final;
        gou::api::Renderer& renderer () const final;
        gou::events::Event* emit () final;
        const gou::api::detail::EventsIterator& events () final;
        entt::registry& registry (gou::api::Registry) final;
        entt::organizer& organizer (gou::api::SystemStage) final;
        entt::entity findEntity (entt::hashed_string) const final;
        const std::string& findEntityName (const components::Named&) const final;
        entt::entity loadEntity (entt::hashed_string) final;
        void mergeEntity (entt::entity, entt::hashed_string, bool) final;
        void registerComponent (gou::api::definitions::Component&) final;
        const std::vector<gou::api::definitions::Component>& getRegisteredComponents () final;
        gou::resources::Handle findResource (entt::hashed_string::hash_type) final;
        gou::resources::Signal findSignal (entt::hashed_string::hash_type) final;

        // Time
        DeltaTime deltaTime () { return m_current_time_delta; }

        // Initialise engine, before modules are loaded
        ImGuiContext* init ();

        // Initialise systems and load game data, after modules are loaded
        void setupGame ();

        // Provide access to input events
        const std::vector<SDL_Event>& inputEvents () {
            return m_input_events;
        }

        // Execute the Taskflow graph of tasks, returns true if still running
        bool execute (Time current_time, DeltaTime delta, uint64_t frame_count);

        // Reset the engine internal state
        void reset ();

        // Call all modules that are added as a specific engine hook
        template <gou::api::Module::CallbackMasks Hook, typename... T> void callModuleHook (T... args) {
            using CM = gou::api::Module::CallbackMasks;
            if constexpr (Hook == CM::BEFORE_FRAME) {
                for (auto& mod : m_hooks_beforeFrame) {
                    mod->on_before_frame(args...);
                }
            } else if constexpr (Hook == CM::AFTER_FRAME) {
                for (auto& mod : m_hooks_afterFrame) {
                    mod->on_after_frame(args...);
                }
            } else if constexpr (Hook == CM::LOAD_SCENE) {
                for (auto& mod : m_hooks_loadScene) {
                    mod->on_load_scene(args...);
                }
            } else if constexpr (Hook == CM::UNLOAD_SCENE) {
                for (auto& mod : m_hooks_unloadScene) {
                    mod->on_unload_scene(args...);
                }
            } else if constexpr (Hook == CM::PREPARE_RENDER) {
                for (auto& mod : m_hooks_prepareRender) {
                    mod->on_prepare_render(args...);
                }
            } else if constexpr (Hook == CM::BEFORE_RENDER) {
                for (auto& mod : m_hooks_beforeRender) {
                    mod->on_before_render(args...);
                }
            } else if constexpr (Hook == CM::AFTER_RENDER) {
                for (auto& mod : m_hooks_afterRender) {
                    mod->on_after_render(args...);
                }
            } else if constexpr (Hook == CM::BEFORE_UPDATE) {
                for (auto& mod : m_hooks_beforeUpdate) {
                    mod->on_before_update(args...);
                }
            }
        }

        // Register module hooks
        void registerModule (std::uint32_t, gou::api::Module*);

        template <typename... Args>
        gou::events::Event& emit (Args&&... args) {
            return gou::api::helpers::emitEvent(*this, std::forward<Args>(args)...);
        }

        enum class EntityLoadType {
            LoadToScene,
            LoadToPrototype,
        };
        // Load component and add it to entity
        void loadComponent (EntityLoadType, entt::hashed_string, entt::entity, const void*);

    private:
        struct NamedEntityInfo {
            entt::entity entity;
            std::string name;
        };

        // ECS registries to manage all entities
        entt::registry m_registry;
        entt::registry m_background_registry;
        entt::registry m_prototype_registry;

        spp::sparse_hash_map<entt::hashed_string::hash_type, gou::api::definitions::LoaderFn, helpers::Identity> m_component_loaders;
        std::vector<gou::api::definitions::Component> m_component_definitions;
        spp::sparse_hash_map<entt::hashed_string::hash_type, NamedEntityInfo, helpers::Identity> m_named_entities;
        spp::sparse_hash_map<entt::hashed_string::hash_type, entt::entity, helpers::Identity> m_prototype_entities;
        world::SceneManager m_scene_manager;
        const std::string m_empty_string = {};

        // System and task scheduling
        spp::sparse_hash_map<gou::api::SystemStage, entt::organizer> m_organizers;
        tf::Taskflow m_coordinator;
        tf::Executor m_executor;

        enum class SystemStatus {
            Running,
            Stopped,
        };
        SystemStatus m_system_status;

        // Timing
        DeltaTime m_current_time_delta = 0;

        // Module Hooks
        std::vector<gou::api::Module*> m_hooks_beforeFrame;
        std::vector<gou::api::Module*> m_hooks_afterFrame;
        std::vector<gou::api::Module*> m_hooks_beforeUpdate;
        std::vector<gou::api::Module*> m_hooks_loadScene;
        std::vector<gou::api::Module*> m_hooks_unloadScene;
        std::vector<gou::api::Module*> m_hooks_prepareRender;
        std::vector<gou::api::Module*> m_hooks_beforeRender;
        std::vector<gou::api::Module*> m_hooks_afterRender;

        // Render state
        gou::api::Renderer* m_renderer;
        graphics::Sync* m_graphics_sync;

        // Physics state
        physics::Context* m_physics_context;

        // Input
        SDL_GameController* m_game_controller;
        std::vector<SDL_Event> m_input_events;

        // Events
        gou::api::detail::EventsIterator m_events_iterator;
        std::vector<EventPool*> m_event_pools;
        EventPool m_event_pool;

        // Implement API interface
        void* allocModule (std::size_t bytes) final;
        void deallocModule (void* ptr) final;

        // Add a module to be called by a specific engine hook
        void addModuleHook (gou::api::Module::CallbackMasks hook, gou::api::Module* module);

        // Create task execution graph
        void createTaskGraph ();

        // Load game data and initialise games first scene
        void setupInitialScene();

        // Process input events
        void handleInput ();

        // Make emitted events available to read and make a fresh event queue available to emit to
        void pumpEvents ();

        // Emit an event directly to the global pool (warning: unsynchronised)
        template <typename... Args> void internalEmplaceEvent (Args&&... args) {
            m_event_pool.emplace(std::forward<Args>(args)...);
        }

        // Update the global event pool iterator to see all events in the global pool
        void refreshEventsIterator ();

        // Merge a prototype entity into an entity
        void mergeEntityInternal (entt::entity, entt::entity, bool);

        // Copy all entities from one registry to another
        void copyRegistry (entt::registry& from, entt::registry& to);

        // Callbacks to manage Named entities
        void onAddNamedEntity (entt::registry&, entt::entity);
        void onRemoveNamedEntity (entt::registry&, entt::entity);

        // Callbacks to manage prototype entities
        void onAddPrototypeEntity (entt::registry&, entt::entity);
        void onRemovePrototypeEntity (entt::registry&, entt::entity);
    };

} // core::
