#pragma once

#include "gou_engine.hpp"

namespace resources {

    using Type = gou::resources::Type;
    using Handle = gou::resources::Handle;

    namespace internal {
        void access (Handle, Type, void(*fn)(void*));
        template <typename T> gou::resources::Type type () {
            return gou::resources::internal::type<T>();
        }
    }

    void init ();
    void term ();

    void declare (entt::hashed_string name, Type type, const std::string& file);
    Handle load (entt::hashed_string name);
    Handle load (entt::hashed_string name, Type type, const std::string& file);
    void unload (entt::hashed_string name);

    bool ready (Handle handle);

    template <typename T, typename Fn> void access (Handle handle, Fn fn) {
        internal::access(handle, internal::type<T>(), [&fn](void* ptr){
            fn(*static_cast<T*>(ptr));
        });
    }
}

