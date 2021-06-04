#pragma once

// Common API between engine and modules
#include "api.hpp"

#include <components/core.hpp>

///////////////////////////////////////////////////////////////////////////////
// Implementation Details, not part of public API
///////////////////////////////////////////////////////////////////////////////

using namespace entt::literals;

// Macro to generate constexpr function hasMember_`Function`<T>() which returns true if T::`Function``Args` exists, otherwise false
#define HAS_MEMBER_FUNCTION(Function, Args)                                         \
namespace supports_detail {                                                         \
  template<typename U>                                                              \
  struct hasMember_ ## Function {                                                   \
  private:                                                                          \
    template<typename>                                                              \
    static constexpr std::false_type test(...);                                     \
    template<typename T = U>                                                        \
    static decltype((std::declval<T>().Function Args), std::true_type{}) test(int); \
  public:                                                                           \
    static constexpr bool value = decltype(test<U>(0))::value;                      \
  }; } template<typename T> constexpr bool hasMember_ ## Function ()                \
  {return supports_detail::hasMember_ ## Function<T>::value;}

// Module-specific API
namespace gou {

    #include "helpers.hpp"

///////////////////////////////////////////////////////////////////////////////
// Public Module API
///////////////////////////////////////////////////////////////////////////////

    using Renderer = api::Renderer;

    // API for manipulating scenes
    class Scene {
    public:        
        /*
         * Create a new empty entity
         */
        entt::entity create () {
            return m_registry.create();
        }

        /*
         * Create a new empty named entity
         */
        entt::entity create (entt::hashed_string name) {
            auto entity = m_registry.create();
            m_registry.emplace<components::Named>(entity, name);
            return entity;
        }

        /*
         * Look up a named entity by name
         */
        entt::entity find (entt::hashed_string name) {
            return m_engine.findEntity(name);
        }

        /*
         * Look up entity by Named, returning its human-readable string name
         */
        const std::string& readableName (const components::Named& named) {
            return m_engine.findEntityName(named);
        }

        /*
         * Test if an entity is still valid
         */
        bool valid (entt::entity entity) {
            return m_registry.valid(entity);
        }

        /*
         * Destroy an entity, invalidating it
         */
        void destroy(entt::entity entity) {
            m_registry.destroy(entity);
        }

        /*
         * Add (or replace, if it exists already) a component to an entity
         * Returns a (possibly const) reference to the new component
         */
        template <typename Component, typename... Args>
        decltype(auto) add(entt::entity entity, Args... args) {
            return m_registry.emplace_or_replace<Component>(entity, args...);
        }

        /*
         * Test if an entity has all of the specified components
         */
        template <typename... Components>
        bool has_all(entt::entity entity) {
            return m_registry.all_of<Components...>(entity);
        }

        /*
         * Test if an entity has any of the specified components
         */
        template <typename... Components>
        bool has_any(entt::entity entity) {
            return m_registry.any_of<Components...>(entity);
        }

        /*
         * Get a component from an entity (must exist)
         * Returns a (possibly const) reference to the component
         */
        template <typename Component>
        decltype(auto) get(entt::entity entity) {
            return m_registry.get<Component>(entity);
        }

        /*
         * Get a pointer to a components from an entity, nullptr if the component does not exist
         */
        template <typename Component>
        Component* try_get(entt::entity entity) {
            return m_registry.try_get<Component>(entity);
        }

        /*
         * Remove a component from an entity
         */
        template <typename Component>
        void remove(entt::entity entity) {
            m_registry.remove_if_exists<Component>(entity);
        }

        /*
         * Create a new entity from an entity prototype
         */
        entt::entity loadEntity (entt::hashed_string id) {
            return m_engine.loadEntity(id);
        }

        /*
         * Merge an entity prototype into an existing entity
         * Any components from the entity that are also in the template will be overwritten with the template ones, unless 'overwrite' is unset
         */
        void mergeEntity (entt::entity entity, entt::hashed_string id, bool overwrite=true) {
            m_engine.mergeEntity(entity, id, overwrite);
        }

        /*
         * Change to a different scene (unloads current scene and loads new one)
         */
        void changeToScene (entt::hashed_string scene) {

        }

        /*
         * Emit an event
         */
        template <typename... Args>
        events::Event& emit (Args&&... args) {
            return api::helpers::emitEvent(m_engine, std::forward<Args>(args)...);
        }

        /*
         * Get an iterator to a read-only iterator to events emitted by the previous frame
         */
        helpers::ConstIterator<api::detail::EventsIterator> events () {
            return helpers::const_iterate(m_engine.events());
        }

        /*
         * Access engine time
         * current_time()   seconds since startup
         * delta_time()     seconds since last frame
         * current_frame()  the number of frames since startup
         */
        Time currentTime () const { return m_current_time; }
        DeltaTime deltaTime() const { return m_delta_time; }
        uint64_t currentFrame() const { return m_current_frame; }

        /*
         * Name of the scene
         */
        std::string name () const { return m_scene_name; }

        /*
         * ID of the scene
         * Use to do scene-specific logic: if (scene.id() == "my-scene"_hs) ...
         */ 
        entt::hashed_string id () const { return m_scene_id; }
    
    protected:
        Time m_current_time;
        DeltaTime m_delta_time;
        uint64_t m_current_frame;
        std::string m_scene_name;
        entt::hashed_string m_scene_id;
        Scene(entt::registry& registry, gou::api::Engine& engine)
            : m_registry(registry),
              m_engine(engine)
        {}
    private:
        entt::registry& m_registry;
        gou::api::Engine& m_engine;
    };

    // API for managing engine setup
    class Engine {
    public:
        gou::api::Engine& engine;
        Scene& scene;

        /*
         * Emit an event
         */
        template <typename... Args>
        events::Event& emit (Args&&... args) {
            return api::helpers::emitEvent(engine, std::forward<Args>(args)...);
        }
    };

///////////////////////////////////////////////////////////////////////////////
// Public Module API
///////////////////////////////////////////////////////////////////////////////

    // TODO: Add spdlog helpers

///////////////////////////////////////////////////////////////////////////////
// Implementation Details, not part of public API
///////////////////////////////////////////////////////////////////////////////

    namespace detail {
        HAS_MEMBER_FUNCTION(onLoad,          (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onUnload,        (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onBeforeReload,  (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onAfterReload,   (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onBeforeFrame,   (std::declval<Scene&>()))
        HAS_MEMBER_FUNCTION(onBeforeUpdate,  (std::declval<Scene&>()))
        HAS_MEMBER_FUNCTION(onAfterFrame,    (std::declval<Scene&>()))
        HAS_MEMBER_FUNCTION(onPrepareRender, (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onBeforeRender,  (std::declval<Renderer&>()))
        HAS_MEMBER_FUNCTION(onAfterRender,   (std::declval<Renderer&>()))
        HAS_MEMBER_FUNCTION(onLoadScene,     (std::declval<Scene&>()))
        HAS_MEMBER_FUNCTION(onUnloadScene,   (std::declval<Scene&>()))
    }

    template <typename Derived>
    class Module : public gou::api::Module {
    public:
        Module (const std::string& name, gou::api::Engine& engine) : m_moduleName{name}, m_engine{engine}, m_scene{engine.registry(gou::api::Registry::Runtime), m_engine} {}
        virtual ~Module () {}

///////////////////////////////////////////////////////////////////////////////
// Public Module API
///////////////////////////////////////////////////////////////////////////////

    /*
     * Logging functions
     */
    template <typename... Args> void info  (const std::string& text, Args... args)  {spdlog::info(m_moduleName + text, args...);}
    template <typename... Args> void warn  (const std::string& text, Args... args)  {spdlog::warn(m_moduleName + text, args...);}
    template <typename... Args> void error (const std::string& text, Args... args)  {spdlog::error(m_moduleName + text, args...);}
    template <typename... Args> void debug (const std::string& text, Args... args)  {
#ifdef DEBUG_BUILD
        spdlog::debug(m_moduleName + text, args...);
#endif
    }

///////////////////////////////////////////////////////////////////////////////
// Implementation Details, not part of public API
///////////////////////////////////////////////////////////////////////////////

    private:
        std::uint32_t on_load () final {
            m_scene.set(0, 0, 0);
            if constexpr (detail::hasMember_onLoad<Derived>()) {
                static_cast<Derived*>(this)->onLoad(Engine{m_engine, m_scene});
            }

            uint32_t flags = 0;
            using CM = gou::api::Module::CallbackMasks;
            if constexpr (detail::hasMember_onBeforeUpdate<Derived>()) {
                flags |= helpers::enum_value(CM::BEFORE_UPDATE);
            }
            if constexpr (detail::hasMember_onAfterFrame<Derived>()) {
                flags |= helpers::enum_value(CM::AFTER_FRAME);
            }
            if constexpr (detail::hasMember_onPrepareRender<Derived>()) {
                flags |= helpers::enum_value(CM::PREPARE_RENDER);
            }
            if constexpr (detail::hasMember_onBeforeRender<Derived>()) {
                flags |= helpers::enum_value(CM::BEFORE_RENDER);
            }
            if constexpr (detail::hasMember_onAfterRender<Derived>()) {
                flags |= helpers::enum_value(CM::AFTER_RENDER);
            }
            if constexpr (detail::hasMember_onLoadScene<Derived>()) {
                flags |= helpers::enum_value(CM::LOAD_SCENE);
            }
            if constexpr (detail::hasMember_onUnloadScene<Derived>()) {
                flags |= helpers::enum_value(CM::UNLOAD_SCENE);
            }
            return flags;
        }

        void on_unload () final {
            if constexpr (detail::hasMember_onUnload<Derived>()) {
                static_cast<Derived*>(this)->onUnload(Engine{m_engine, m_scene});
            }
        }

        void on_before_reload () final {
            if constexpr (detail::hasMember_onBeforeReload<Derived>()) {
                static_cast<Derived*>(this)->onBeforeReload(Engine{m_engine, m_scene});
            }
        }

        void on_after_reload () final {
            if constexpr (detail::hasMember_onAfterReload<Derived>()) {
                static_cast<Derived*>(this)->onAfterReload(Engine{m_engine, m_scene});
            }
        }

        void on_before_frame (Time time, DeltaTime delta, uint64_t frame) final {
            m_scene.set(time, delta, frame);
            if constexpr (detail::hasMember_onBeforeFrame<Derived>()) {
                static_cast<Derived*>(this)->onBeforeFrame(m_scene);
            }
        }

        void on_before_update () final {
            if constexpr (detail::hasMember_onBeforeUpdate<Derived>()) {
                static_cast<Derived*>(this)->onBeforeUpdate(m_scene);
            }
        }

        void on_after_frame () final {
            if constexpr (detail::hasMember_onAfterFrame<Derived>()) {
                static_cast<Derived*>(this)->onAfterFrame(m_scene);
            }
        }

        void on_prepare_render () final {
            if constexpr (detail::hasMember_onPrepareRender<Derived>()) {
                static_cast<Derived*>(this)->onPrepareRender(Engine{m_engine, m_scene});
            }
        }

        void on_before_render () final {
            if constexpr (detail::hasMember_onBeforeRender<Derived>()) {
                static_cast<Derived*>(this)->onBeforeRender(m_engine.renderer());
            }
        }

        void on_after_render () final {
            if constexpr (detail::hasMember_onAfterRender<Derived>()) {
                static_cast<Derived*>(this)->onAfterRender(m_engine.renderer());
            }
        }

        void on_load_scene (entt::hashed_string name) final {
            if constexpr (detail::hasMember_onLoadScene<Derived>()) {
                m_scene.setScene(name);
                static_cast<Derived*>(this)->onLoadScene(m_scene);
            }
        }

        void on_unload_scene () final {
            if constexpr (detail::hasMember_onUnloadScene<Derived>()) {
                static_cast<Derived*>(this)->onUnloadScene(m_scene);
            }
        }

        const std::string m_moduleName;
        gou::api::Engine& m_engine;
        class SettableScene : public Scene {
        public:
            SettableScene (entt::registry& registry, gou::api::Engine& engine) : Scene(registry, engine) {}
            ~SettableScene() {}
            void set (Time time, DeltaTime delta, uint64_t frame)  {
                m_current_time = time;
                m_delta_time = delta;
                m_current_frame = frame;
            }
            void setScene (entt::hashed_string name) {
                m_scene_name = name.data();
                m_scene_id = name;
            }
        } m_scene;
    };

    namespace api::detail {
        #include "type_info.hpp"
    }
    struct ctx {
        static gou::api::detail::type_context* ref;
        static gou::api::Module* gou_module;
    };
    #define GOU_MODULE_INIT(NAMESPACE) gou::api::Module* NAMESPACE module_init (const std::string& name, gou::api::Engine* engine)
    GOU_MODULE_INIT();
#ifndef NO_COMPONENTS
    void register_components(api::Engine*);
    #define GOU_REGISTER_COMPONENTS gou::register_components(engine)
#else
    #define GOU_REGISTER_COMPONENTS
#endif
}

template<typename Type>
struct entt::type_seq<Type> {
    [[nodiscard]] static id_type value() ENTT_NOEXCEPT {
        static const entt::id_type value = gou::ctx::ref->value(entt::type_hash<Type>::value());
        return value;
    }
};

///////////////////////////////////////////////////////////////////////////////
// Macro for declaring module class
///////////////////////////////////////////////////////////////////////////////

// Add module class boilerplate
#define GOU_MODULE_CLASS(ClassName) public: ClassName(const std::string& name, gou::api::Engine& e) : gou::Module<ClassName>(name, e) {} virtual ~ClassName() {} private:

// Declare module
#define GOU_MODULE(ClassName) GOU_MODULE_INIT(gou::) { GOU_REGISTER_COMPONENTS; return engine->createModule<ClassName>(name); }


