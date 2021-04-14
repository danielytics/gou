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

    // Wrapper around engine to provide user-API
    class Engine {
    public:
        gou::api::Engine& engine;
    };

    class Scene {

    };

    namespace detail {
        HAS_MEMBER_FUNCTION(onLoad,        (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onUnload,      (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onBeforeReload,(std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onAfterReload, (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onBeforeFrame, (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onBeforeUpdate,(std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onAfterFrame,  (std::declval<Engine>()))
        HAS_MEMBER_FUNCTION(onBeforeRender,())
        HAS_MEMBER_FUNCTION(onAfterRender, ())
        HAS_MEMBER_FUNCTION(onLoadScene,   (std::declval<Engine>(), std::declval<Scene&>()))
        HAS_MEMBER_FUNCTION(onUnloadScene, (std::declval<Engine>(), std::declval<Scene&>()))
    }

    template <typename Derived>
    class Module : public gou::api::Module {
    public:
        Module (gou::api::Engine& engine) : m_engine(engine)
        {

        }

        virtual ~Module ()
        {

        }

    private:
        std::uint32_t on_load () final;
        void on_unload () final;
        void on_before_reload ();
        void on_after_reload () final;
        void on_before_frame (Time time, DeltaTime delta, uint64_t frame);
        void on_before_update ();
        void on_after_frame ();
        void on_before_render ();
        void on_after_render ();
        void on_load_scene ();
        void on_unload_scene ();

        gou::api::Engine& m_engine;
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
