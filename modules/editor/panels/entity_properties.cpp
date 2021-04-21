
#include "entity_properties.hpp"

void EntityPropertiesPanel::render ()
{
    if (m_selected_entity != entt::null) {
        ImGui::Text("Entity Selected");
    }
}
