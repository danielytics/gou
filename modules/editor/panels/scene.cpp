
#include "scene.hpp"

#include <gou.hpp>
#include <components/core.hpp>

void ScenePanel::beforeRender (gou::Engine& engine)
{
	m_entities.clear();
	const auto& view = engine.engine.registry().view<const components::Named>();
	for (auto&& [entity, named] : view.each()) {
		m_entities.push_back({
			engine.engine.findEntityName(named),
			entity,
		});
	}
}

void ScenePanel::render (gou::Renderer& renderer)
{
	// Deselect entity if clicking in the window, but not on an entity
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
		m_selected_entity = entt::null;
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Entities")) {
		for (const auto& info : m_entities) {
			ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
			flags |= (m_selected_entity == info.entity) ? ImGuiTreeNodeFlags_Selected : 0;
			if (ImGui::TreeNodeEx((void*)(std::uint64_t)entt::to_integral(info.entity), flags, "%s", info.name.c_str())) {
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					m_selected_entity = info.entity;
				} else if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
					m_selected_entity = info.entity;
					if (ImGui::BeginPopup("Entity Menu", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings)) {
						if (ImGui::MenuItem("Remove Entity")) {

						}
						ImGui::EndPopup();
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Prototypes")) {
		ImGui::TreePop();
	}
	if (ImGui::BeginPopupContextWindow("Scene Menu", ImGuiMouseButton_Right, false))
	{
		if (ImGui::MenuItem("Add Entity")) {

		}
		ImGui::EndPopup();
	}
	if (ImGui::IsItemClicked(ImGuiMouseButton_Middle)) {
		if (ImGui::BeginPopup("Entity Menu", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings)) {
			if (ImGui::MenuItem("Remove Entity")) {

			}
			ImGui::EndPopup();
		}
	}
}