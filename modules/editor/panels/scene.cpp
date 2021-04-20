
#include "scene.hpp"

#include <gou.hpp>
#include <components/core.hpp>

void ScenePanel::render (gou::Renderer& renderer)
{
    const auto& view = renderer.registry.view<const components::Named>();
    for (auto&& [entity, named] : view.each()) {		
		ImGuiTreeNodeFlags flags = (false ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		[[maybe_unused]] bool opened = ImGui::TreeNodeEx((void*)(std::uint64_t)entt::to_integral(entity), flags, "%s", renderer.entityName(named.name).c_str());
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