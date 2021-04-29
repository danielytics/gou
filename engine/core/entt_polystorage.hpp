#pragma once

#include <entt/core/utility.hpp>
#include <entt/entity/poly_storage.hpp>

template<typename... Type>
entt::type_list<Type...> as_type_list(const entt::type_list<Type...> &);

template<typename Entity>
struct PolyStorage: entt::type_list_cat_t<
    decltype(as_type_list(std::declval<entt::Storage<Entity>>())),
    entt::type_list<
        void(entt::basic_registry<Entity>&, const Entity, const void*),
        void(entt::basic_registry<Entity>&, const Entity, const void*),
        const void*(const Entity) const,
        bool(const Entity) const
    >
> {
    using entity_type = Entity;
    using size_type = std::size_t;

    template<typename Base>
    struct type: entt::Storage<Entity>::template type<Base> {
        static constexpr auto base = decltype(as_type_list(std::declval<entt::Storage<Entity>>()))::size;

        void emplace (entt::basic_registry<Entity>& owner, const entity_type entity, const void* instance) {
            entt::poly_call<base + 0>(*this, owner, entity, instance);
        }

        void replace (entt::basic_registry<Entity>& owner, const entity_type entity, const void* instance) {
            entt::poly_call<base + 1>(*this, owner, entity, instance);
        }

        const void* get (const entity_type entity) const {
            return entt::poly_call<base + 2>(*this, entity);
        }

        bool contains (const entity_type entity) const {
            return entt::poly_call<base + 3>(*this, entity);
        }
    };

    template<typename Type>
    struct members {
        static void emplace (Type& self, entt::basic_registry<Entity>& owner, const entity_type entity, const void* instance) {
            self.emplace(owner, entity, *static_cast<const typename Type::value_type*>(instance));
        }

        static void replace (Type& self, entt::basic_registry<Entity>& owner, const entity_type entity, const void* instance) {
            auto new_data = static_cast<const typename Type::value_type*>(instance);
            self.patch(owner, entity, [new_data](auto&& old_data){
                std::memcpy(&old_data, new_data, sizeof(typename Type::value_type));
            });
        }

        static const typename Type::value_type* get(const Type& self, const entity_type entity) {
            return &self.get(entity);
        }

        static bool contains (const Type& self, const entity_type entity) {
            return self.contains(entity);
        }
    };

    template<typename Type>
    using impl = entt::value_list_cat_t<
        typename entt::Storage<Entity>::template impl<Type>,
        entt::value_list<
            &members<Type>::emplace,
            &members<Type>::replace,
            &members<Type>::get,
            &members<Type>::contains
        >
    >;
};

template<typename Entity>
struct entt::poly_storage_traits<Entity> {
    using storage_type = entt::poly<PolyStorage<Entity>>;
};
