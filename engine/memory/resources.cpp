
#include "resources.hpp"
#include "resource_loaders.hpp"

using Handle = resources::Handle;
using Type = resources::Type;

namespace gou::resources::internal {
    struct Handle {
        struct {
            std::uint32_t unused : 7;
            std::uint32_t type : 5;
            std::uint32_t instance : 16;
        } bits;
    };
}
using HandleImpl = gou::resources::internal::Handle;

resources::ResourceTypes g_resource_types;

struct ResourceHandleEntry {
    HandleImpl handle;
    void* data;
    std::atomic_uint32_t* ref_handles;
};
spp::sparse_hash_map<entt::hashed_string::hash_type, ResourceHandleEntry, helpers::Identity> g_resource_instances;
spp::sparse_hash_map<std::uint32_t, ResourceHandleEntry*> g_resource_instance_data;

void resources::internal::access (Handle handle, Type expected_type, void(*fn)(void*))
{
    Type actual_type =  Type{handle->bits.type};
    if (actual_type == expected_type) {
        auto it = g_resource_instance_data.find(*reinterpret_cast<std::uint32_t*>(&handle->bits));
        if (it != g_resource_instance_data.end()) {
            fn(it->second->data);
        }
    }
}

void resources::init ()
{
    resources::loaders::init(g_resource_types);
}

void resources::term ()
{
    resources::loaders::term(g_resource_types);
}

Handle resources::load (entt::hashed_string name)
{
    auto it = g_resource_instances.find(name.value());
    if (it != g_resource_instances.end()) {
        auto& handle = it->second;
        handle.ref_handles->fetch_add(1);
        return &handle.handle;
    }
    return nullptr;
}

Handle resources::load (entt::hashed_string name, Type type, const std::string& filename)
{
    auto handle = resources::load(name);
    if (handle == nullptr) {
        auto type_idx = helpers::enum_value(type);
        auto& def = g_resource_types[type_idx];
        auto instance_idx = ++def.instance_count;
        void* data;
        if (def.free_list.empty()) {
            // There's nothing in the free list, take from pool
            data = def.loader->loadResource(filename);
        } else {
            // Otherwise, pop an item from the free list
            data = def.loader->loadResource(def.free_list.back(), filename);
            def.free_list.pop_back();
        }
        g_resource_instances[name.value()] = ResourceHandleEntry{{{0, type_idx, instance_idx}}, data, new std::atomic_uint32_t};
        auto& instance = g_resource_instances[name.value()];
        g_resource_instance_data[*reinterpret_cast<std::uint32_t*>(&handle->bits)] = &instance;
        handle = &instance.handle;
    }
    return handle;
}

void resources::unload (entt::hashed_string name)
{
    auto it = g_resource_instances.find(name.value());
    if (it != g_resource_instances.end()) {
        it->second.ref_handles->fetch_sub(1);
    }
}

void collectGarbage ()
{
    for (auto& [_, value] : g_resource_instances) {
        if (value.ref_handles->load() == 0) {

        }
    }
}

bool resources::ready (Handle handle)
{
    return false;
}

