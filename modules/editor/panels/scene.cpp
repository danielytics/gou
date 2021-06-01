
#include "scene.hpp"

#include <gou/gou.hpp>
#include <components/core.hpp>

void ScenePanel::beforeRender (gou::Engine& engine)
{
	if (m_scene_action != SceneAction::None) {
		switch (m_scene_action) {
		case SceneAction::NewEntity:
		{
			auto entity = engine.scene.create();
			auto name = std::string{"Entity "} + std::to_string(entt::to_integral(entity)+1);
			engine.scene.add<components::Named>(entity, entt::hashed_string{name.c_str()});
			break;
		}
		default:
			break;
		};
		m_scene_action = SceneAction::None;
	}

	m_entities.clear();
	const auto& view = engine.engine.registry(gou::api::Engine::Registry::Runtime).view<const components::Named>();
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
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
		m_selected_entity = entt::null;
	}

	bool context_menu = false;
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Entities")) {
		for (const auto& info : m_entities) {
			ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
			flags |= (m_selected_entity == info.entity) ? ImGuiTreeNodeFlags_Selected : 0;
			if (ImGui::TreeNodeEx((void*)(std::uint64_t)entt::to_integral(info.entity), flags, "%s", info.name.c_str())) {
				ImGui::TreePop();
			}
			if (ImGui::IsItemHovered()) {
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					m_selected_entity = info.entity;
					m_selected_name = info.name;
				}
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Prototypes")) {
		ImGui::TreePop();
	}
	if (ImGui::BeginPopupContextWindow("Scene Menu", ImGuiMouseButton_Right, false)) {
		if (ImGui::MenuItem("Add Entity")) {
			m_scene_action = SceneAction::NewEntity;
		}
		ImGui::EndPopup();
	}
}