#pragma once

#include <cstdint>
#include <string>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <spdlog/spdlog.h>

#include "types.hpp"

struct ImGuiContext;

namespace components {
    struct Named;
}

namespace gou::api {
    class Engine;
    namespace detail {
        class type_context;
        struct ModuleInfo {
            // ModuleInfo (const std::string& name, Engine* engine, std::shared_ptr<spdlog::logger> logger) : name{name}, engine{engine}, logger{logger} {}
            const std::string name;
            Engine* const engine;
            const std::shared_ptr<spdlog::logger> logger;
            ImGuiContext* imgui_context;
        };

        struct EventsIterator {
            using Type = gou::events::Event;
            Type* data;
            std::size_t count;
        };
    }

    enum class SystemStage {
        GameLogic = 0,
        Update = 1
    };

    class Renderer {
    public:
        virtual ~Renderer() {}
        virtual void setViewport (const glm::vec4&) = 0;
    };

    // The module-provided API to the engine
    class Module {
    public:
        virtual ~Module() {}
        enum class CallbackMasks : std::uint32_t {
            // load, unload and before_frame are always called. reload functions are handled by the module (not the engine)
            BEFORE_FRAME    = 0x00, // Doesn't need to set a bit because its always registered for every module
            BEFORE_UPDATE   = 0x01,
            AFTER_FRAME     = 0x02,
            PREPARE_RENDER  = 0x04, 
            BEFORE_RENDER   = 0x08,
            AFTER_RENDER    = 0x10,
            LOAD_SCENE      = 0x20,
            UNLOAD_SCENE    = 0x40,

            /** Notes on data access.
             * It is always safe to access data passed into the hooks as arguments. Sharing data between hooks, however, must follow some rules.
             * The GOU game engine executes hooks in one of two thread contexts: 'engine' and 'renderer'
             * You must not share data (eg member variables of your Module instance) between these two contexts, as they may run concurrently, unless
             * the data is atomic or protected by a lock, however locks should be avoided to maintain high performance.
             * 
             * Hooks that execute in the 'engine' context are:
             *  onLoad, onUnload, onBeforeReload, onAfterReload, onBeforeFrame, onBeforeUpdate, onAfterFrame, onLoadScene, onUnloadScene and onPrepareRender
             * 
             * Hooks that execute in the 'renderer' context are:
             *  onBeforeRender, onAfterRender and onPrepareRender
             * 
             * It is safe to access both engine and render data from onPrepareRender (technically it runs in the 'renderer' context, but inside a critical
             * section, allowing safe access to the engine -- however, logic in onPrepareRender should be kept to a minimum).
             * 
             * Systems execute in the 'engine' context, however, it is not safe to access the engine from inside a system as they may be scheduled in parallel.
             * Instead, systems should communicate either through their writeable components on the entities they process, or by setting local member data in
             * their Module, that you know isn't being accessed by another system and then using a hook (eg onBeforeUpdate or onAfterFrame) to communicate the
             * data elsewhere.
             * 
             * It is safe to emit events at any time from the 'engine' context, even in systems. The 'render' context (excluding onPrepareRender) should not
             * emit events.
             */
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
        virtual void on_prepare_render () = 0;
        virtual void on_before_render () = 0;
        virtual void on_after_render () = 0;
        // Scene setup and teardown. Use these to set up scene logic and scene-specific systems.
        virtual void on_load_scene (entt::hashed_string) = 0;
        virtual void on_unload_scene () = 0;
    };

    // The engine-provided API to modules
    class Engine {
    public:
        virtual ~Engine() {}
        // Used internally by module boilerplate to ensure ECS type ID's are consistent across modules
        virtual detail::type_context* type_context() const = 0;

        // Used internally to register a modules callback hooks
        virtual void registerModule(std::uint32_t, Module*) = 0;

        // Used internally to provide the module with access to the renderer
        virtual Renderer& renderer () const = 0;

        // Emit an event
        virtual events::Event* emit () = 0;

        // Access events emitted last frame
        virtual const detail::EventsIterator& events () = 0;

        // Access to the ECS registry
        virtual entt::registry& registry() = 0;

        // Access ECS organizers through which to register systems
        virtual entt::organizer& organizer(std::uint32_t) = 0;

        // Find a named entity
        virtual entt::entity findEntity (entt::hashed_string) const = 0;

        // Get the string name of a named entity
        virtual const std::string& findEntityName (const components::Named& named) const = 0;

        // Load an entity from a template
        virtual entt::entity loadEntity (entt::hashed_string) = 0;

        // Merge a template into an entity
        virtual void mergeEntity (entt::entity, entt::hashed_string, bool) = 0;

        // Register a component loader
        using LoaderFn = void(*)(Engine* engine, entt::registry& registry, const void* table, entt::entity entity);
        virtual void registerLoader(entt::hashed_string, LoaderFn) = 0;

        // Get the prototype registry, used by the component loader setup code, not meant for module users
        virtual entt::registry& prototypeRegistry () = 0;

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

    namespace helpers {
        template <typename... Args>
        gou::events::Event& emitEvent (Engine& engine, Args&&... args) {
            return *new (engine.emit())gou:: events::Event{args...};
        }
    }
}