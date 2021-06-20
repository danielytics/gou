
#include "resource_loaders.hpp"
#include "resources.hpp"

#include "graphics/graphics.hpp"
#include "graphics/mesh.hpp"

#define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#ifdef RELEASE_BUILD
    #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#endif
#include <tiny_gltf.h>

struct Model {

};

class ModelLoader : public resources::loaders::TypedResourceLoader<ModelLoader, Model> {
public:
    ModelLoader (std::uint32_t pool_size) : pool(pool_size) {}
    virtual ~ModelLoader () {}

    void load (Model* ptr, const std::string& filename) {
        tinygltf::Model model;
        std::string err;
        std::string warn;
        bool ret;
        const std::string input = helpers::readToString(filename);
        std::string_view ext = std::string_view(filename).substr(filename.size() - 4, 4);
        if (ext == ".glb") {
            const unsigned char* buffer = reinterpret_cast<const unsigned char*>(input.data());
            ret = loader.LoadBinaryFromMemory(&model, &err, &warn, buffer, input.size());
        } else if (ext == "gltf") {
            ret = loader.LoadASCIIFromString(&model, &err, &warn, input.data(), input.size(), "");
        } else {
            spdlog::error("Unknown Mesh file type: {}", ext);
            return;
        }
        if (!warn.empty()) {
            spdlog::warn("Warn: {}", warn);
        }

        if (!err.empty()) {
            spdlog::error("Err: {}", err);
        }

        if (!ret) {
            spdlog::error("Failed to parse glTF");
        } else {
            // for (auto& mesh : model.meshes) {
            // }
        }
    }
    void unload (Model* ptr) {
        // Unload data from ptr
        pool.discard(ptr);
    }

private:
    tinygltf::TinyGLTF loader;

    memory::Pool<Model, memory::AlignSIMD> pool;
    void* allocate () final {
        return pool.allocate();
    }
};




template <entt::id_type ID, typename T> void add (resources::ResourceTypes& types)
{
    const std::uint32_t pool_size = entt::monostate<ID>();
    types[helpers::enum_value(T::Type)].loader = new T(pool_size);
}

void resources::loaders::init (resources::ResourceTypes& types)
{
    add<"resources/pool-size/models"_hs, ModelLoader>(types);
}

void resources::loaders::term (resources::ResourceTypes& types)
{
    for (auto& type : types) {
        if (type.loader) {
            delete type.loader;
            type.loader = nullptr;
        }
    }
}
