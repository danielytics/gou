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

    namespace types {
        enum class Type {
            Vec2,
            Vec3,
            Vec4,
            UInt8,
            UInt16,
            UInt32,
            UInt64,
            Int8,
            Int16,
            Int32,
            Int64,
            Byte,
            Flags8,
            Flags16,
            Flags32,
            Flags64,
            Resource,
            TextureResource,
            MeshResource,
            Entity,
            Float,
            Double,
            Bool,
            Event,
            Ref,
            HashedString,
            RGB,
            RGBA,
            Signal,
        };
    }

    namespace resources {

        struct Handle {

        };

        struct EntitySetHandle {

        };

        struct Signal {
            std::uint32_t index;
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
