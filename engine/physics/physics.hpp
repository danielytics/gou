#pragma once

#include <gou_engine.hpp>

namespace core {
    class Engine;
}

namespace physics {

    struct Context;

    using view_flush_dynamic = entt::view<entt::exclude_t<>, components::Position, const components::physics::DynamicBody>;
    using view_flush_kinematic = entt::view<entt::exclude_t<>, components::Position, const components::physics::KinematicBody>;

    Context* init (core::Engine&);
    void prepare (Context*, entt::registry&);
    void simulate (Context*);
    void flush_dynamic (Context*, view_flush_dynamic);
    void flush_kinematic (Context*, view_flush_kinematic);
    void term (Context*);

} // physics::
