#pragma once

#include "gou_engine.hpp"
#include <gou_api.hpp>
#include "world/scenes.hpp"

#include <SDL.h>

#include <taskflow/taskflow.hpp>

namespace graphics {
    struct Sync;
}
namespace physics {
    struct Context;
}

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

        // Implement API interface
        gou::api::detail::type_context* type_context() final;
        void registerModule (std::uint32_t, gou::api::Module*) final;
        gou::events::Event* event () final;
        entt::registry& registry() final;
        entt::registry& prototypeRegistry () final;
        entt::organizer& organizer(std::uint32_t) final;
        entt::entity findEntity (entt::hashed_string) final;
        entt::entity loadEntity (entt::hashed_string) final;
        void mergeEntity (entt::entity, entt::hashed_string, bool) final;
        void registerLoader(entt::hashed_string, gou::api::Engine::LoaderFn) final;
        gou::resources::Handle findResource (entt::hashed_string::hash_type) final;

        // Time
        DeltaTime deltaTime () { return m_current_time_delta; }

        // Initialise systems and load game data
        void setupGame (graphics::Sync*);

        // Process input events
        void handleInput (bool& running);

        // Provide access to input events
        const std::vector<SDL_Event>& inputEvents () {
            return m_input_events;
        }

        // Execute the Taskflow graph of tasks
        void execute (Time current_time, DeltaTime delta, uint64_t frame_count);

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

        enum class EntityLoadType {
            LoadToScene,
            LoadToPrototype,
        };
        // Load component and add it to entity
        void loadComponent (EntityLoadType, entt::hashed_string, entt::entity, const void*);

    private:
        // ECS registry to manage all entities
        entt::registry m_registry;
        entt::registry m_prototype_registry;
        spp::sparse_hash_map<entt::hashed_string::hash_type, gou::api::Engine::LoaderFn, helpers::Identity> m_component_loaders;
        spp::sparse_hash_map<entt::hashed_string::hash_type, entt::entity, helpers::Identity> m_named_entities;
        spp::sparse_hash_map<entt::hashed_string::hash_type, entt::entity, helpers::Identity> m_prototype_entities;
        world::SceneManager m_scene_manager;

        // System and task scheduling
        spp::sparse_hash_map<uint32_t, entt::organizer> m_organizers;
        tf::Taskflow m_coordinator;
        tf::Executor m_executor;

        // Timing
        DeltaTime m_current_time_delta = 0;

        // Module Hooks
        std::vector<gou::api::Module*> m_hooks_beforeFrame;
        std::vector<gou::api::Module*> m_hooks_afterFrame;
        std::vector<gou::api::Module*> m_hooks_beforeUpdate;
        std::vector<gou::api::Module*> m_hooks_loadScene;
        std::vector<gou::api::Module*> m_hooks_unloadScene;
        std::vector<gou::api::Module*> m_hooks_beforeRender;
        std::vector<gou::api::Module*> m_hooks_afterRender;

        // Render state
        graphics::Sync* m_state_sync;

        // Physics state
        physics::Context* m_physics_context;

        // Input
        SDL_GameController* m_game_controller;
        std::vector<SDL_Event> m_input_events;

        // Implement API interface
        void* allocModule (std::size_t bytes) final;
        void deallocModule (void* ptr) final;

        // Add a module to be called by a specific engine hook
        void addModuleHook (gou::api::Module::CallbackMasks hook, gou::api::Module* module);
        // Create task execution graph
        void createTaskGraph ();
        // Load game data and initialise games first scene
        void setupInitialScene();
        // Make emitted events available to read and make a fresh event queue available to emit to
        void pumpEvents ();
        // Merge a prototype entity into an entity
        void mergeEntityInternal (entt::entity, entt::entity, bool);

        // Callbacks to manage Named entities
        void onAddNamedEntity (entt::registry&, entt::entity);
        void onRemoveNamedEntity (entt::registry&, entt::entity);
        // Callbacks to manage prototype entities
        void onAddPrototypeEntity (entt::registry&, entt::entity);
        void onRemovePrototypeEntity (entt::registry&, entt::entity);
    };

} // core::
