
#include "entity_properties.hpp"

void EntityPropertiesPanel::load (gou::Engine& engine) {
    for (const auto& component : engine.engine.getRegisteredComponents()) {
        // debug("Component: {}", component.name);
        for (const auto& attribute : component.attributes) {
            // debug("  - attribute: {} (offset: {}, type: {})", attribute.name, attribute.offset, type_to_string[attribute.type]);
        }
    }
}

void EntityPropertiesPanel::beforeRender (gou::Engine& engine, gou::Scene& scene)
{
    if (m_selected_entity != entt::null) {
        maybe_clear();
        make_editor<components::Position>(engine);
        make_editor<components::Transform>(engine);

        if (m_entity_action != EntityAction::None) {
            switch (m_entity_action) {
            case EntityAction::Rename:
            {
                auto name = entt::hashed_string{m_name_buffer};
                // Can only rename if the new name doesn't already exist
                if (scene.find(name) == entt::null) {
                    // Remove old name
                    scene.remove<components::Named>(m_selected_entity);
                    // Add new name
                    scene.add<components::Named>(m_selected_entity, name);
                    // Set the name for display
                    m_entity_name = std::string{m_name_buffer};
                }
                break;
            }
            case EntityAction::Delete:
                scene.destroy(m_selected_entity);
                m_selected_entity = entt::null;
                break;
            case EntityAction::RemoveComponent:
                m_action_component->remove(scene, m_selected_entity);
                break;
            default:
                break;
            };
            m_entity_action = EntityAction::None;
        }
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
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(m_entity_name.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Rename")) {
                ImGui::OpenPopup("Rename");
                std::strncpy(m_name_buffer, m_entity_name.c_str(), sizeof(m_name_buffer));
            }

            ImGui::TableNextColumn();
            if (ImGui::Button("X")) {
                ImGui::OpenPopup("Delete?");
            }

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();

            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Are you sure that you wish to delete this entity?\nThis operation cannot be undone!\n\n");
                ImGui::Separator();
                if (ImGui::Button("Delete", ImVec2(120, 0))) {
                    m_entity_action = EntityAction::Delete;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::EndPopup();
            }

            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Entity Name:");
                ImGui::SameLine();
                ImGui::InputText("##name", m_name_buffer, sizeof(m_name_buffer));
                ImGui::Separator();
                if (ImGui::Button("Rename", ImVec2(120, 0))) {
                    m_entity_action = EntityAction::Rename;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::EndPopup();
            }

            ImGui::EndTable();
        }
        for (auto& [_, editor] : m_data_editors) {
            editor->doRender();
            if (editor->action() != EntityAction::None) {
                m_entity_action = editor->action();
                m_action_component = editor;
            }
        }
    }
}
