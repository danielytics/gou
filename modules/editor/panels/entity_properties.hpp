#pragma once

#include "panel.hpp"

class EntityPropertiesPanel : public Panel<EntityPropertiesPanel> {
public:
    EntityPropertiesPanel () : Panel<EntityPropertiesPanel>("Entity Properties") {}
    ~EntityPropertiesPanel() {}

    void render ();

    void select (entt::entity entity) { m_selected_entity = entity; }

private:
    entt::entity m_selected_entity;
};
