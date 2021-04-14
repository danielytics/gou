
#include "engine.hpp"

#include <entt/core/type_info.hpp>
namespace gou::api::detail {
    #include <type_info.hpp>
}


gou::api::detail::type_context* core::Engine::type_context() {
    return gou::api::detail::type_context::instance();
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
