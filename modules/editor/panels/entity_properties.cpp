
#include "entity_properties.hpp"

void EntityPropertiesPanel::render ()
{
    for (auto& [_, editor] : m_data_editors) {
        editor->doRender();
    }
}

void EntityPropertiesPanel::beforeRender (gou::Engine& engine)
{
    if (m_selected_entity != entt::null) {
        maybe_clear();
        make_editor<components::Position>(engine);
        make_editor<components::Transform>(engine);
        make_editor<components::TimeAware>(engine);
    } else {
        maybe_clear();
    }
}
