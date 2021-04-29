
#include "engine.hpp"

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
    m_prototype_registry.visit(prototype_entity, [this,entity,prototype_entity,overwrite_components](const auto info) {
        auto&& prototype_storage = m_prototype_registry.storage(info);
        auto&& scene_storage = m_registry.storage(info);
        if (scene_storage->contains(entity)) {
            if (overwrite_components) {
                scene_storage->replace(m_registry, entity, prototype_storage->get(prototype_entity));
            }
        } else {
            scene_storage->emplace(m_registry, entity, prototype_storage->get(prototype_entity));
        }
    });
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
