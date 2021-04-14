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
        gou::api::detail::type_context* type_context();
        entt::registry& registry();
        entt::organizer& organizer(std::uint32_t);

    private:
        entt::registry m_registry;
        entt::organizer m_organizer;

        void* allocModule (std::size_t bytes);
        void deallocModule (void* ptr);
    };
}

