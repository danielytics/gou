
#include "entity_properties.hpp"

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

void EntityPropertiesPanel::render ()
{
    if (m_selected_entity != entt::null) {
        if (ImGui::BeginTable("Entity Table", 3, ImGuiTableFlags_None)) {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 15.0f);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Name");
            ImGui::TableNextColumn();
            if (true) {
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("<name>");
                ImGui::SameLine();
                if (ImGui::Button("Rename")) {

                }
            } else {
                char buffer[32];
                if (ImGui::InputText("##t", buffer, 32)) {

                }
            }

            ImGui::TableNextColumn();
            if (ImGui::Button("X")) {

            }
            ImGui::EndTable();
        }
        for (auto& [_, editor] : m_data_editors) {
            editor->doRender();
        }
    }
}
