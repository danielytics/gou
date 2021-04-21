
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

    for (const auto& info : m_entities) {
		ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= (m_selected_entity == info.entity) ? ImGuiTreeNodeFlags_Selected : 0;
		bool opened = ImGui::TreeNodeEx((void*)(std::uint64_t)entt::to_integral(info.entity), flags, "%s", info.name.c_str());
		if (opened) {
			ImGui::TreePop();
		}
		if (ImGui::IsItemClicked())
		{
			m_selected_entity = info.entity;
		}
    }
}