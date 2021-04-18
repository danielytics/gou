#pragma once

#include <cstdint>

using Time = float;
using DeltaTime = Time;

#include <glm/glm.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>

namespace gou {

    namespace resources {

        struct Handle {

        };

        struct EntitySetHandle {

        };

    }

    namespace events {

        struct Event {
            entt::hashed_string::hash_type type;
            entt::entity source;
            entt::entity target;
            glm::vec3 attributes;
            int32_t value;
            uint32_t handle;
        }; // 32 bytes

    }

}
