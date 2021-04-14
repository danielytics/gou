
#include <gou.hpp>

#include <cr.h>
#include <entt/core/type_info.hpp>

/********************************************************************************
 * Plugin entrypoint and setup
 ********************************************************************************/

gou::api::detail::type_context* gou::ctx::ref;
gou::api::Module* gou::ctx::gou_module;


CR_EXPORT int cr_main(cr_plugin* ctx, cr_op operation)
{
    if(!gou::ctx::ref) {
        auto engine = static_cast<gou::api::Engine*>(ctx->userdata);
        gou::ctx::ref = engine->type_context();
        gou::ctx::gou_module = gou::module_init(engine);
        gou::ctx::gou_module->on_load();
    }
    switch (operation) {
        case CR_LOAD:
        {
            gou::ctx::gou_module->on_after_reload();
            break;
        }
        case CR_UNLOAD:
        {
            gou::ctx::gou_module->on_before_reload();
            break;
        }
        case CR_STEP:
        {
            break;
        }
        case CR_CLOSE:
        {
            gou::ctx::gou_module->on_unload();
            break;
        }
    }
    return 0;
}

/********************************************************************************
 * Module API implementation
 ********************************************************************************/

template <class Derived> std::uint32_t gou::Module<Derived>::on_load () {
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

template <class Derived> void gou::Module<Derived>::on_unload () {
    if constexpr (detail::hasMember_onUnload<Derived>()) {
        static_cast<Derived*>(this)->onUnload(Engine{m_engine});
    }
}

template <class Derived> void gou::Module<Derived>::on_before_reload () {
    if constexpr (detail::hasMember_onBeforeReload<Derived>()) {
        static_cast<Derived*>(this)->onBeforeReload(Engine{m_engine});
    }
}

template <class Derived> void gou::Module<Derived>::on_after_reload () {
    if constexpr (detail::hasMember_onAfterReload<Derived>()) {
        static_cast<Derived*>(this)->onAfterReload(Engine{m_engine});
    }
}

template <class Derived> void gou::Module<Derived>::on_before_frame (Time time, DeltaTime delta, uint64_t frame) {
    // current_time = time;
    // delta_time = delta;
    // current_frame = frame;
    if constexpr (detail::hasMember_onBeforeFrame<Derived>()) {
        static_cast<Derived*>(this)->onBeforeFrame(Engine{m_engine});
    }
}

template <class Derived> void gou::Module<Derived>::on_before_update () {
    if constexpr (detail::hasMember_onBeforeUpdate<Derived>()) {
        static_cast<Derived*>(this)->onBeforeUpdate(Engine{m_engine});
    }
}

template <class Derived> void gou::Module<Derived>::on_after_frame () {
    if constexpr (detail::hasMember_onAfterFrame<Derived>()) {
        static_cast<Derived*>(this)->onAfterFrame(Engine{m_engine});
    }
}

template <class Derived> void gou::Module<Derived>::on_before_render () {
    if constexpr (detail::hasMember_onBeforeRender<Derived>()) {
        static_cast<Derived*>(this)->onBeforeRender();
    }
}

template <class Derived> void gou::Module<Derived>::on_after_render () {
    if constexpr (detail::hasMember_onAfterRender<Derived>()) {
        static_cast<Derived*>(this)->onAfterRender();
    }
}

template <class Derived> void gou::Module<Derived>::on_load_scene () {
    if constexpr (detail::hasMember_onLoadScene<Derived>()) {
        Scene scene;
        static_cast<Derived*>(this)->onLoadScene(Engine{m_engine}, scene);
    }
}

template <class Derived> void gou::Module<Derived>::on_unload_scene () {
    if constexpr (detail::hasMember_onUnloadScene<Derived>()) {
        Scene scene;
        static_cast<Derived*>(this)->onUnloadScene(Engine{m_engine}, scene);
    }
}
