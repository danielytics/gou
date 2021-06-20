#pragma once

#include "gou_engine.hpp"

namespace resources {
    namespace loaders {
        class ResourceLoader {
        public:
            virtual ~ResourceLoader () {}

            virtual void* loadResource (void* buffer, const std::string& filename) = 0;
            virtual void unloadResource (void* buffer) = 0;

            void* loadResource (const std::string& filename) {
                return loadResource(allocate(), filename);
            }
        private:
            virtual void* allocate () = 0;
        };

        template <typename Derived, typename T>
        class TypedResourceLoader : public ResourceLoader {
        public:         
            static const gou::resources::Type Type = gou::resources::internal::type<T>();
            virtual ~TypedResourceLoader () {}

            void* loadResource (void* buffer, const std::string& filename) final {
                static_cast<Derived*>(this)->load(static_cast<T*>(buffer), filename);
                return buffer;
            }
            void unloadResource (void* buffer) final {
                T* ptr = static_cast<T*>(buffer);
                static_cast<Derived*>(this)->unload(ptr);
            }
        };
    }
    struct ResourceTypeEntry {
        loaders::ResourceLoader* loader = nullptr;
        std::vector<void*> free_list;
        std::uint32_t instance_count = 0;
    };
    using ResourceTypes = std::array<ResourceTypeEntry, helpers::enum_value(gou::resources::Type::None) - 1>;

    namespace loaders {
        void init (ResourceTypes&);
        void term (resources::ResourceTypes&);
    }
}

