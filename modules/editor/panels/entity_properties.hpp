#pragma once

#include "panel.hpp"
#include <functional>
#include <entt/core/type_info.hpp>
#include <sparsepp/spp.h>

#include "component_editor.hpp"

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
    std::vector<DataEditor> m_data_editors;
    spp::sparse_hash_map<entt::id_type, const gou::api::definitions::Component*> m_components_by_type;
    std::vector<std::pair<std::string, std::vector<const gou::api::definitions::Component*>>> m_components_by_category;

    EntityAction m_entity_action = EntityAction::None;
    const gou::api::definitions::Component* m_active_compoment = nullptr;
    std::string m_entity_name;
    char m_name_buffer[32] = "\0";
};
