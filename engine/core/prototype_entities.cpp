
#include "engine.hpp"

#include <entt/core/utility.hpp>
#include <entt/entity/poly_storage.hpp>

template<typename... Type>
entt::type_list<Type...> as_type_list(const entt::type_list<Type...> &);

template<typename Entity>
struct PolyStorage: entt::type_list_cat_t<
    decltype(as_type_list(std::declval<entt::Storage<Entity>>())),
    entt::type_list<
        void(const Entity *, const Entity *, void *),
        void(entt::basic_registry<Entity> &, const Entity, const void *),
        const void *(const Entity) const,
        void(entt::basic_registry<Entity> &) const
    >
> {
    using entity_type = Entity;
    using size_type = std::size_t;

    template<typename Base>
    struct type: entt::Storage<Entity>::template type<Base> {
        static constexpr auto base = decltype(as_type_list(std::declval<entt::Storage<Entity>>()))::size;

        void remove(entt::basic_registry<Entity> &owner, const entity_type *first, const entity_type *last) {
            entt::poly_call<base + 0>(*this, first, last, &owner);
        }

        void emplace(entt::basic_registry<Entity> &owner, const entity_type entity, const void *instance) {
            entt::poly_call<base + 1>(*this, owner, entity, instance);
        }

        const void * get(const entity_type entity) const {
            return entt::poly_call<base + 2>(*this, entity);
        }

        void copy_to(entt::basic_registry<Entity> &other) const {
            entt::poly_call<base + 3>(*this, other);
        }
    };

    template<typename Type>
    struct members {
        static void emplace(Type &self, entt::basic_registry<Entity> &owner, const entity_type entity, const void *instance) {
            self.emplace(owner, entity, *static_cast<const typename Type::value_type *>(instance));
        }

        static const typename Type::value_type * get(const Type &self, const entity_type entity) {
            return &self.get(entity);
        }

        static void copy_to(const Type &self, entt::basic_registry<entity_type> &other) {
            other.template insert<typename Type::value_type>(self.data(), self.data() + self.size(), self.raw(), self.raw() + self.size());
        }
    };

    template<typename Type>
    using impl = entt::value_list_cat_t<
        typename entt::Storage<Entity>::template impl<Type>,
        entt::value_list<
            &Type::template remove<const entity_type *>,
            &members<Type>::emplace,
            &members<Type>::get,
            &members<Type>::copy_to
        >
    >;
};

template<typename Entity>
struct entt::poly_storage_traits<Entity> {
    using storage_type = entt::poly<PolyStorage<Entity>>;
};


entt::entity core::Engine::loadEntity (entt::hashed_string prototype_id)
{
    auto it = m_prototype_entities.find(prototype_id);
    if (it != m_prototype_entities.end()) {
        auto entity = m_registry.create();
        mergeEntityInternal(entity, it->second, true);
        return entity;
    }
    return entt::null;
}

void core::Engine::mergeEntity (entt::entity entity, entt::hashed_string prototype_id, bool overwrite_components)
{
    auto it = m_prototype_entities.find(prototype_id);
    if (it != m_prototype_entities.end()) {
        mergeEntityInternal(entity, it->second, overwrite_components);
    }
}

void core::Engine::mergeEntityInternal (entt::entity entity, entt::entity prototype_entity, bool overwrite_components)
{
    // m_prototype_registry.visit(prototype_entity, [this](const auto info) {
    //     auto &&storage = m_prototype_registry.storage(info);
    //     storage->emplace(m_registry, entity, storage->get(prototype_entity));
    // });
}


void core::Engine::onAddPrototypeEntity (entt::registry& registry, entt::entity entity)
{
    const auto& prototype_id = registry.get<core::EntityPrototypeID>(entity);
    auto it = m_prototype_entities.find(prototype_id.id);
    if (it != m_prototype_entities.end()) {
        // Already exists, destroy the old one before replacing it with the new one.
        m_prototype_registry.destroy(it->second);
    }
    m_prototype_entities[prototype_id.id] = entity;
}

void core::Engine::onRemovePrototypeEntity (entt::registry& registry, entt::entity entity)
{
    const auto& prototype_id = registry.get<core::EntityPrototypeID>(entity);
    m_prototype_entities.erase(prototype_id.id);
}
