#pragma once

#include <cstdint>
#include <string>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <spdlog/spdlog.h>

#include "types.hpp"

struct ImGuiContext;

namespace gou::api {
    class Engine;
    namespace detail {
        class type_context;
        struct ModuleInfo {
            // ModuleInfo (const std::string& name, Engine* engine, std::shared_ptr<spdlog::logger> logger) : name{name}, engine{engine}, logger{logger} {}
            const std::string name;
            Engine* const engine;
            const std::shared_ptr<spdlog::logger> logger;
        };
    }

    enum class SystemStage {
        GameLogic = 0,
        Update = 1
    };

    // The module-provided API to the engine
    class Module {
    public:
        enum class CallbackMasks : std::uint32_t {
            // load, unload and before_frame are always called. reload functions are handled by the module (not the engine)
            BEFORE_FRAME  = 0x00, // Doesn't need to set a bit because its always registered for every module
            BEFORE_UPDATE = 0x01,
            AFTER_FRAME   = 0x02,
            BEFORE_RENDER = 0x04,
            AFTER_RENDER  = 0x08,
            LOAD_SCENE    = 0x10,
            UNLOAD_SCENE  = 0x20,
        };
        // Module lifecycle. Use these to setup and shutdown your module, setting up global (non-scene-specific) systems.
        virtual std::uint32_t on_load () = 0;
        virtual void on_unload () = 0;
        // Dev-mode hot-code reload lifecycle functions.
        virtual void on_before_reload () = 0; // Before hot code reload, use to persist data
        virtual void on_after_reload () = 0; // After hot code reload, use to reload data
        // Logic hooks. Use these to add custom logic on a per-frame basis.
        virtual void on_before_frame (Time, DeltaTime, uint64_t) = 0;
        virtual void on_before_update () = 0;
        virtual void on_after_frame () = 0;
        // Rendering hooks. Use these to add custom rendering, including dev tool UI.
        virtual void on_before_render () = 0;
        virtual void on_after_render () = 0;
        // Scene setup and teardown. Use these to set up scene logic and scene-specific systems.
        virtual void on_load_scene () = 0;
        virtual void on_unload_scene () = 0;
    };

    // The engine-provided API to modules
    class Engine {
    public:
        // Used internally by module boilerplate to ensure ECS type ID's are consistent across modules
        virtual detail::type_context* type_context() = 0;

        // Used internally to register a modules callback hooks
        virtual void registerModule(std::uint32_t, Module*) = 0;

        // Access to the ECS registry
        virtual entt::registry& registry() = 0;

        // Access ECS organizers through which to register systems
        virtual entt::organizer& organizer(std::uint32_t) = 0;

        // Find a named entity
        virtual entt::entity findEntity (entt::hashed_string) = 0;

        // Load an entity from a template
        virtual entt::entity loadEntity (entt::hashed_string) = 0;

        // Merge a template into an entity
        virtual void mergeEntity (entt::entity, entt::hashed_string, bool) = 0;

        // Register a component loader
        using LoaderFn = void(*)(Engine* engine, entt::registry& registry, const void* table, entt::entity entity);
        virtual void registerLoader(entt::hashed_string, LoaderFn) = 0;

        // Retrieve a resource handle by name
        virtual gou::resources::Handle findResource (entt::hashed_string::hash_type) = 0;

        // Internal module creation and destruction API
        template <class Module> Module* createModule (const std::string& name) {
            return new (allocModule(sizeof(Module))) Module(name, *this);
        }
        template <class Module> void destroyModule (Module* mod) {
            mod->~Module();
            deallocModule(mod);
        }
    private:
        // Allow engine to decide where the module classes are allocated
        virtual void* allocModule (std::size_t) = 0;
        virtual void deallocModule (void*) = 0;
    };
}
