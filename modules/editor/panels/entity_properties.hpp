#pragma once

#include "panel.hpp"

class EntityPropertiesPanel : public Panel<EntityPropertiesPanel> {
public:
    EntityPropertiesPanel () : Panel<EntityPropertiesPanel>("Entity Properties") {}
    ~EntityPropertiesPanel() {}

    void render ();
};
