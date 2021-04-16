
#include "engine.hpp"

#include <entt/core/type_info.hpp>
namespace gou::api::detail {
    #include <type_info.hpp>
}


gou::api::detail::type_context* core::Engine::type_context() {
    return gou::api::detail::type_context::instance();
}

void core::Engine::registerModule (std::uint32_t flags, gou::api::Module* mod)
{
    using CM = gou::api::Module::CallbackMasks;
    for (auto hook : {CM::AFTER_FRAME, CM::LOAD_SCENE, CM::UNLOAD_SCENE, CM::BEFORE_UPDATE, CM::BEFORE_RENDER, CM::AFTER_RENDER}) {
        if (flags & utilities::enum_value(hook)) {
            addModuleHook(hook, mod);
        }
    }
}

entt::registry& core::Engine::registry() {
    return m_registry;
}

entt::organizer& core::Engine::organizer(std::uint32_t) {
    return m_organizer;
}

void* core::Engine::allocModule (std::size_t bytes) {
    return reinterpret_cast<void*>(new std::byte[bytes]);
}
void core::Engine::deallocModule (void* ptr) {
    delete [] reinterpret_cast<std::byte*>(ptr);
}

void core::Engine::addModuleHook (gou::api::Module::CallbackMasks hook, gou::api::Module* mod) {
    using CM = gou::api::Module::CallbackMasks;
    switch (hook) {
        case CM::AFTER_FRAME:
            hooks_afterFrame.push_back(mod);
            break;
        case CM::BEFORE_UPDATE:
            hooks_beforeUpdate.push_back(mod);
            break;
        case CM::BEFORE_RENDER:
            hooks_beforeRender.push_back(mod);
            break;
        case CM::AFTER_RENDER:
            hooks_afterRender.push_back(mod);
            break;
        case CM::LOAD_SCENE:
            hooks_loadScene.push_back(mod);
            break;
        case CM::UNLOAD_SCENE:
            hooks_unloadScene.push_back(mod);
            break;
    };
}
