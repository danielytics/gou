#pragma once

#include <cstdint>

using Time = float;
using DeltaTime = Time;

#include <glm/glm.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>

[[nodiscard]] constexpr entt::hashed_string::hash_type operator"" _event(const char *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_string{str}.value();
}

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
            glm::vec3 attributes;
            int32_t value;
            uint32_t handle;
        }; // 28 bytes (16 events fit in 7 cache lines)

    }

}
