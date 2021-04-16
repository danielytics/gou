#pragma once

#include "gou_engine.hpp"
#include <gou_api.hpp>

namespace core {
    /**
     * Engine is the API through which functionality is registered with the engine.
     * This is responsible for accessing the ECS registry, registering up ECS system tasks, registering components and providing "core" services.
     * It is the API through which modules interact with the engine during setup, but is not typically used during gameplay runtime.
     */
    class Engine : public gou::api::Engine {
    public:
        // Implement API interface
        gou::api::detail::type_context* type_context() final;
        void registerModule (std::uint32_t, gou::api::Module*) final;
        entt::registry& registry() final;
        entt::organizer& organizer(std::uint32_t) final;

        // Call all modules that are added as a specific engine hook
        template <gou::api::Module::CallbackMasks Hook, typename... T> void callModuleHook (T... args) {
            using CM = gou::api::Module::CallbackMasks;
            if constexpr (Hook == CM::AFTER_FRAME) {
                for (auto& mod : hooks_afterFrame) {
                    mod->on_after_frame(args...);
                }
            } else if constexpr (Hook == CM::LOAD_SCENE) {
                for (auto& mod : hooks_loadScene) {
                    mod->on_load_scene(args...);
                }
            } else if constexpr (Hook == CM::UNLOAD_SCENE) {
                for (auto& mod : hooks_unloadScene) {
                    mod->on_unload_scene(args...);
                }
            } else if constexpr (Hook == CM::BEFORE_RENDER) {
                for (auto& mod : hooks_beforeRender) {
                    mod->on_before_render(args...);
                }
            } else if constexpr (Hook == CM::AFTER_RENDER) {
                for (auto& mod : hooks_afterRender) {
                    mod->on_after_render(args...);
                }
            } else if constexpr (Hook == CM::BEFORE_UPDATE) {
                for (auto& mod : hooks_beforeUpdate) {
                    mod->on_before_update(args...);
                }
            }
        }

    private:
        entt::registry m_registry;
        entt::organizer m_organizer;

        // Module Hooks
        std::vector<gou::api::Module*> hooks_afterFrame;
        std::vector<gou::api::Module*> hooks_beforeUpdate;
        std::vector<gou::api::Module*> hooks_loadScene;
        std::vector<gou::api::Module*> hooks_unloadScene;
        std::vector<gou::api::Module*> hooks_beforeRender;
        std::vector<gou::api::Module*> hooks_afterRender;

        // Implement API interface
        void* allocModule (std::size_t bytes) final;
        void deallocModule (void* ptr) final;

        // Add a module to be called by a specific engine hook
        void addModuleHook (gou::api::Module::CallbackMasks hook, gou::api::Module* module);
    };
}

