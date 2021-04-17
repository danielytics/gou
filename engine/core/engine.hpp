#pragma once

#include "gou_engine.hpp"
#include <gou_api.hpp>

#include <taskflow/taskflow.hpp>

namespace core {
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
        entt::registry& registry() final;
        entt::organizer& organizer(std::uint32_t) final;
        entt::entity findEntity (entt::hashed_string) final;
        entt::entity loadEntity (entt::hashed_string) final;
        void mergeEntity (entt::entity, entt::hashed_string, bool) final;
        void registerLoader(entt::hashed_string, gou::api::Engine::LoaderFn) final;
        gou::resources::Handle findResource (entt::hashed_string::hash_type) final;

        // Execute the Taskflow graph of tasks
        void execute (Time current_time, DeltaTime delta, uint64_t frame_count);
        void reset ();

        // Call all modules that are added as a specific engine hook
        template <gou::api::Module::CallbackMasks Hook, typename... T> void callModuleHook (T... args) {
            using CM = gou::api::Module::CallbackMasks;
            if constexpr (Hook == CM::BEFORE_FRAME) {
                for (auto& mod : m_hooks_afterFrame) {
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

    private:
        // ECS registry to manage all entities
        entt::registry m_registry;

        // System and task scheduling
        std::map<uint32_t, entt::organizer> m_organizers;
        std::vector<tf::Taskflow*> m_taskflows;
        tf::Taskflow m_coordinator;
        tf::Executor m_executor;

        // Timing
        DeltaTime current_time_delta = 0;

        // Module Hooks
        std::vector<gou::api::Module*> m_hooks_beforeFrame;
        std::vector<gou::api::Module*> m_hooks_afterFrame;
        std::vector<gou::api::Module*> m_hooks_beforeUpdate;
        std::vector<gou::api::Module*> m_hooks_loadScene;
        std::vector<gou::api::Module*> m_hooks_unloadScene;
        std::vector<gou::api::Module*> m_hooks_beforeRender;
        std::vector<gou::api::Module*> m_hooks_afterRender;

        // Implement API interface
        void* allocModule (std::size_t bytes) final;
        void deallocModule (void* ptr) final;

        // Add a module to be called by a specific engine hook
        void addModuleHook (gou::api::Module::CallbackMasks hook, gou::api::Module* module);
        // Create task execution graph
        void createTaskGraph ();
        // Make emitted events available to read and make a fresh event queue available to emit to
        void pumpEvents ();
    };
}

