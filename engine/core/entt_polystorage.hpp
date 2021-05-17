#pragma once

#include <entt/core/utility.hpp>
#include <entt/entity/poly_storage.hpp>

template<typename... Type>
entt::type_list<Type...> as_type_list(const entt::type_list<Type...> &);

template<typename Entity>
struct PolyStorage: entt::type_list_cat_t<
    decltype(as_type_list(std::declval<entt::Storage<Entity>>())),
    entt::type_list<
        void(entt::basic_registry<Entity>&, const Entity, const void*, bool),
        const void*(const Entity) const
    >
> {
    using entity_type = Entity;
    using size_type = std::size_t;

    template<typename Base>
    struct type: entt::Storage<Entity>::template type<Base> {
        static constexpr auto base = decltype(as_type_list(std::declval<entt::Storage<Entity>>()))::size;

        void copy (entt::basic_registry<Entity>& owner, const entity_type entity, const void* instance, bool overwrite_existing) {
            entt::poly_call<base + 0>(*this, owner, entity, instance, overwrite_existing);
        }

        const void* get (const entity_type entity) const {
            return entt::poly_call<base + 1>(*this, entity);
        }

    };

    template<typename Type>
    struct members {
        static void copy (Type& self, entt::basic_registry<Entity>& owner, const entity_type entity, const void* instance, bool overwrite_existing) {
            auto new_data = static_cast<const typename Type::value_type*>(instance);
            if (self.contains(entity)) {
                if (overwrite_existing) {
                    self.patch(owner, entity, [new_data](auto&& old_data){
                        std::memcpy(&old_data, new_data, sizeof(typename Type::value_type));
                    });
                }
            } else {
                self.emplace(owner, entity, *new_data);
            }
        }

        static const typename Type::value_type* get(const Type& self, const entity_type entity) {
            return &self.get(entity);
        }
    };

    template<typename Type>
    using impl = entt::value_list_cat_t<
        typename entt::Storage<Entity>::template impl<Type>,
        entt::value_list<
            &members<Type>::copy,
            &members<Type>::get
        >
    >;
};

template<typename Entity>
struct entt::poly_storage_traits<Entity> {
    using storage_type = entt::poly<PolyStorage<Entity>>;
};
