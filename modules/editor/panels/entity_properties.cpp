
#include "entity_properties.hpp"

void EntityPropertiesPanel::load (gou::Engine& engine) {
    using Component = gou::api::definitions::Component;
    spp::sparse_hash_map<std::string, std::vector<const Component*>> components_by_category;
    for (const auto& component : engine.engine.getRegisteredComponents()) {
        m_components_by_type[component.type_id] = &component;
        if (! (component.category == "core" && component.name == "Named")) { // Don't include "Named" as an addable component
            components_by_category[component.category].push_back(&component);
        }
    }
    // Construct list of components, excluding "core"
    auto core_components = components_by_category["core"];
    components_by_category.erase("core");
    m_components_by_category = {{"", {}}}; // Force item to be at the front, will be replaced with "core" after sorting
    for (auto& [category, components] : components_by_category) {
        m_components_by_category.emplace_back(std::make_pair(category, components));
    }
    // Sort list by category
    std::sort(m_components_by_category.begin(), m_components_by_category.end(), [](auto& a, auto& b){
        return a.first < b.first;
    });
    // Add core to the front of the list (overwriting the empty named category from above)
    m_components_by_category[0] = {"core", core_components};
    // Sort compoents within each category by name
    for (auto& [_, components] : m_components_by_category) {
        std::sort(components.begin(), components.end(), [](auto a, auto b){
            return a->name < b->name;
        });
    }
}

void EntityPropertiesPanel::beforeRender (gou::Engine& engine, gou::Scene& scene)
{
    static auto& registry = engine.engine.registry(gou::api::Registry::Runtime);
    if (m_selected_entity != entt::null) {

        if (m_selected_entity != m_prev_selected_entity) {
            m_data_editors.clear();
            registry.visit(m_selected_entity, [this](auto type){
                auto it = m_components_by_type.find(type.seq());
                if (it != m_components_by_type.end()) {
                    const gou::api::definitions::Component& component = *it->second;
                    if (component.name != "Named" && component.attached_to_entity(registry, m_selected_entity)) {
                        char* component_ptr = component.getter ? component.getter(registry, m_selected_entity) : nullptr;
                        m_data_editors.emplace_back(component, component_ptr);
                    }
                }
            });
            std::sort(m_data_editors.begin(), m_data_editors.end(), [](auto& a, auto& b){ return a.component_def().name < b.component_def().name; });
            m_prev_selected_entity = m_selected_entity;
        }
        for (auto& editor : m_data_editors) {
            editor.update();
        }

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
            case EntityAction::AddComponent:
                m_active_compoment->manage(registry, m_selected_entity, gou::api::definitions::ManageOperation::Add);
                m_prev_selected_entity = entt::null; // Force a refresh of components
                break;
            case EntityAction::RemoveComponent:
                m_active_compoment->manage(registry, m_selected_entity, gou::api::definitions::ManageOperation::Remove);
                m_prev_selected_entity = entt::null; // Force a refresh of components
                break;
            default:
                break;
            };
            m_entity_action = EntityAction::None;
        }
    } else {
        m_data_editors.clear();
        m_prev_selected_entity = entt::null;
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

            if (ImGui::BeginPopupContextWindow("Edit Components", ImGuiMouseButton_Right, false)) {
                if (ImGui::BeginMenu("Add Component")) {
                    for (auto& [category, components] : m_components_by_category) {
                        if (ImGui::BeginMenu(category.c_str())) {
                            for (auto component : components) {
                                if (ImGui::MenuItem(component->name.c_str())) {
                                    m_entity_action = EntityAction::AddComponent;
                                    m_active_compoment = component;
                                }
                            }
                            ImGui::EndMenu();
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
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
        
        for (auto& editor : m_data_editors) {
            editor.render();
            if (editor.removed()) {
                m_active_compoment = &editor.component_def();
                m_entity_action = EntityAction::RemoveComponent;
            }
        }
    }
}
