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

namespace graphics {
    class Model;
    class Mesh;
    class Material;
    class Texture;
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

        enum class Type : std::uint32_t {
            ///////////////////////////////////////////////////////////////////////
            // Graphics Resources /////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////

            // Models and meshes
            Model = 0,      // A model, loaded from a gLTF file (includes meshes and materials)
            StaticMesh,     // A single mesh, without material, cannot be modified (stored on GPU)
            DynamicMesh,    // A single mesh, withotu material, can be modified (stored in main memory)

            // Materials and textures
            Material,       // A single material, without mesh
            Texture,        // A single texture
            TextureArray,   // An array of textures

            ///////////////////////////////////////////////////////////////////////
            // Audio Resources    /////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////

            AudioClip,      // An audio clip, fully loaded to memory
            AudioStream,    // An audio stream, dynamically streamed from disk

            ///////////////////////////////////////////////////////////////////////
            // Other              /////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////

            None = 32,      // No resource
        };

        namespace internal {
            struct Handle;

            template <typename T> constexpr Type type () { return Type::None; }
            template <> constexpr Type type<graphics::Model> () { return Type::Model; }
            template <> constexpr Type type<graphics::Mesh> () { return Type::StaticMesh; }
            template <> constexpr Type type<graphics::Material> () { return Type::Material; }
            template <> constexpr Type type<graphics::Texture> () { return Type::Texture; }
        }
        using Handle = internal::Handle*;

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
