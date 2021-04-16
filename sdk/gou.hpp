#pragma once

// Common API between engine and modules
#include "gou_api.hpp"

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

    #include <utilities.hpp>

    // API for managing engine setup
    class Engine {
    public:
        gou::api::Engine& engine;
    };

    // API for accessing rendering
    class Renderer {
    public:
    };

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
            m_registry.emplace<api::components::Named>(entity, name);
            return entity;
        }

        /*
         * Look up a named entity by name
         */
        entt::entity find (entt::hashed_string name) {
            return m_engine.findEntity(name);
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
         * Create a new entity from an entity template
         */
        entt::entity loadEntity (entt::hashed_string id) {
            return m_engine.loadEntity(id);
        }

        /*
         * Merge an entity template into an existing entity
         * Any components from the entity that are also in the template will be overwritten with the template ones, unless 'overwrite' is unset
         */
        void mergeEntity (entt::entity entity, entt::hashed_string id, bool overwrite=true) {
            m_engine.mergeEntity(entity, id, overwrite);
        }

        // Time accessors
        const Time current_time () {return m_current_time;}
        const DeltaTime delta_time() {return m_delta_time;}
        const uint64_t current_frame() {return m_current_frame;}
    
    protected:
        Time m_current_time;
        DeltaTime m_delta_time;
        uint64_t m_current_frame;
        Scene(entt::registry& registry, gou::api::Engine& engine)
            : m_registry(registry),
              m_engine(engine)
        {}
    private:
        entt::registry& m_registry;
        gou::api::Engine& m_engine;
    };

    namespace detail {
        HAS_MEMBER_FUNCTION(onLoad,        (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onUnload,      (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onBeforeReload,(std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onAfterReload, (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onBeforeFrame, (std::declval<Scene&>()))
        HAS_MEMBER_FUNCTION(onBeforeUpdate,(std::declval<Scene&>()))
        HAS_MEMBER_FUNCTION(onAfterFrame,  (std::declval<Scene&>()))
        HAS_MEMBER_FUNCTION(onBeforeRender,(std::declval<Renderer>()))
        HAS_MEMBER_FUNCTION(onAfterRender, (std::declval<Renderer>()))
        HAS_MEMBER_FUNCTION(onLoadScene,   (std::declval<Scene&>()))
        HAS_MEMBER_FUNCTION(onUnloadScene, (std::declval<Scene&>()))
    }

    template <typename Derived>
    class Module : public gou::api::Module {
    public:
        Module (gou::api::Engine& engine) : m_engine{engine}, m_scene{engine.registry(), m_engine} {}
        virtual ~Module () {}

    private:
        std::uint32_t on_load () final {
            if constexpr (detail::hasMember_onLoad<Derived>()) {
                static_cast<Derived*>(this)->onLoad(Engine{m_engine});
            }

            uint32_t flags = 0;
            using CM = gou::api::Module::CallbackMasks;
            if constexpr (detail::hasMember_onBeforeUpdate<Derived>()) {
                flags |= utilities::enum_value(CM::BEFORE_UPDATE);
            }
            if constexpr (detail::hasMember_onAfterFrame<Derived>()) {
                flags |= utilities::enum_value(CM::AFTER_FRAME);
            }
            if constexpr (detail::hasMember_onBeforeRender<Derived>()) {
                flags |= utilities::enum_value(CM::BEFORE_RENDER);
            }
            if constexpr (detail::hasMember_onAfterRender<Derived>()) {
                flags |= utilities::enum_value(CM::AFTER_RENDER);
            }
            if constexpr (detail::hasMember_onLoadScene<Derived>()) {
                flags |= utilities::enum_value(CM::LOAD_SCENE);
            }
            if constexpr (detail::hasMember_onUnloadScene<Derived>()) {
                flags |= utilities::enum_value(CM::UNLOAD_SCENE);
            }
            return flags;
        }

        void on_unload () final {
            if constexpr (detail::hasMember_onUnload<Derived>()) {
                static_cast<Derived*>(this)->onUnload(Engine{m_engine});
            }
        }

        void on_before_reload () final {
            if constexpr (detail::hasMember_onBeforeReload<Derived>()) {
                static_cast<Derived*>(this)->onBeforeReload(Engine{m_engine});
            }
        }

        void on_after_reload () final {
            if constexpr (detail::hasMember_onAfterReload<Derived>()) {
                static_cast<Derived*>(this)->onAfterReload(Engine{m_engine});
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

        void on_before_render () final {
            if constexpr (detail::hasMember_onBeforeRender<Derived>()) {
                static_cast<Derived*>(this)->onBeforeRender(Renderer{});
            }
        }

        void on_after_render () final {
            if constexpr (detail::hasMember_onAfterRender<Derived>()) {
                static_cast<Derived*>(this)->onAfterRender(Renderer{});
            }
        }

        void on_load_scene () final {
            if constexpr (detail::hasMember_onLoadScene<Derived>()) {
                static_cast<Derived*>(this)->onLoadScene(m_scene);
            }
        }

        void on_unload_scene () final {
            if constexpr (detail::hasMember_onUnloadScene<Derived>()) {
                static_cast<Derived*>(this)->onUnloadScene(m_scene);
            }
        }

        gou::api::Engine& m_engine;
        class SettableScene : public Scene {
        public:
            SettableScene (entt::registry& registry, gou::api::Engine& engine) : Scene(registry, engine) {}
            ~SettableScene() {}
            void set (Time time, DeltaTime delta, uint64_t frame)  {
                m_current_frame = time;
                m_delta_time = delta;
                m_current_frame = frame;
            }
        } m_scene;
    };

    gou::api::Module* module_init (api::Engine*);

    namespace api::detail {
        #include <type_info.hpp>
    }
    struct ctx {
        static gou::api::detail::type_context* ref;
        static gou::api::Module* gou_module;
    };
}

// Declare module
#define GOU_MODULE(ClassName) gou::api::Module* gou::module_init (gou::api::Engine* engine) { return engine->createModule<ClassName>(); }
// Add module class boilerplate
#define GOU_CLASS(ClassName) public: ClassName(gou::api::Engine& e) : gou::Module<ClassName>(e) {} virtual ~ClassName() {} private:

template<typename Type>
struct entt::type_seq<Type> {
    [[nodiscard]] static id_type value() ENTT_NOEXCEPT {
        static const entt::id_type value = gou::ctx::ref->value(entt::type_hash<Type>::value());
        return value;
    }
};
