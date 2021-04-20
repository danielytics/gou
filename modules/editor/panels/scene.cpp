
#include "scene.hpp"

#include <gou.hpp>
#include <components/core.hpp>

void ScenePanel::beforeRender (gou::Engine& engine)
{
	entities.clear();
	const auto& view = engine.engine.registry().view<const components::Named>();
	for (auto&& [entity, named] : view.each()) {
		entities.push_back({
			engine.engine.findEntityName(named),
			entity,
		});
	}
}

void ScenePanel::render (gou::Renderer& renderer)
{
    for (auto& info : entities) {
		ImGuiTreeNodeFlags flags = (false ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		[[maybe_unused]] bool opened = ImGui::TreeNodeEx((void*)(std::uint64_t)entt::to_integral(info.entity), flags, "%s", info.name.c_str());
		if (ImGui::IsItemClicked())
		{
			// m_SelectionContext = entity;
		}

		// bool entityDeleted = false;
		// if (ImGui::BeginPopupContextItem())
		// {
		// 	if (ImGui::MenuItem("Delete Entity"))
		// 		entityDeleted = true;

		// 	ImGui::EndPopup();
		// }

		// if (opened)
		// {
		// 	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		// 	bool opened = ImGui::TreeNodeEx((void*)9817239, flags, named.name.data());
		// 	if (opened)
		// 		ImGui::TreePop();
		// 	ImGui::TreePop();
		// }

		// if (entityDeleted)
		// {
		// 	m_Context->DestroyEntity(entity);
		// 	if (m_SelectionContext == entity)
		// 		m_SelectionContext = {};
		// }
    }
}