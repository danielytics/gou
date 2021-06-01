#pragma once

#include "panel.hpp"
#include <functional>
#include <entt/core/type_info.hpp>
#include <unordered_map>

enum class EntityAction {
    None,
    Rename,
    Delete,
    RemoveComponent,
};
#include "component_editors.hpp"

class EntityPropertiesPanel : public Panel<EntityPropertiesPanel> {
public:
    EntityPropertiesPanel () : Panel<EntityPropertiesPanel>("Entity Properties") {}
    virtual ~EntityPropertiesPanel() {}

    void load (gou::Engine&);

    void beforeRender (gou::Engine& engine, gou::Scene& scene);
    void render ();

    void select (entt::entity entity, const std::string& name) {
        m_selected_entity = entity;
        m_entity_name = name;
    }

    entt::entity selected () const { return m_selected_entity; }

private:
    entt::entity m_selected_entity = entt::null;
    entt::entity m_prev_selected_entity = entt::null;
    std::unordered_map<entt::id_type, DataEditor*> m_data_editors;

    EntityAction m_entity_action = EntityAction::None;
    DataEditor* m_action_component = nullptr;
    std::string m_entity_name;
    char m_name_buffer[32] = "\0";

    template <typename Component>
    void make_editor (gou::Engine& engine) {
        auto& registry = engine.engine.registry(gou::api::Engine::Registry::Runtime);
        auto type = entt::type_id<Component>().seq();
        if (registry.any_of<Component>(m_selected_entity)) {
            auto it = m_data_editors.find(type);
            if (it == m_data_editors.end()) {
                // Add new editor
                auto [iterator, _] = m_data_editors.emplace(type, new TemlpatedDataEditor<Component>());
                it = iterator;
            }
            TemlpatedDataEditor<Component>* editor = static_cast<TemlpatedDataEditor<Component>*>(it->second);
            auto& position = registry.get<Component>(m_selected_entity);
            editor->update(engine, position);
        } else {
            // Remove existing editor, if any
            auto it = m_data_editors.find(type);
            if (it != m_data_editors.end()) {
                delete it->second;
                m_data_editors.erase(it);
            }
        }
    }

    void maybe_clear ()
    {
        if (m_selected_entity != m_prev_selected_entity) {
            m_prev_selected_entity = m_selected_entity;
            for (auto& [_, editor] : m_data_editors) {
                delete editor;
            }
            m_data_editors.clear();
        }
    }
};
