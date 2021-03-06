#pragma once

#include "panel.hpp"

enum class SceneAction {
    None,
    NewEntity,
};

struct EntityInfo {
    std::string name;
    entt::entity entity;
};

class ScenePanel : public Panel<ScenePanel> {
public:
    ScenePanel () : Panel<ScenePanel>("Scene") {}
    ~ScenePanel() {}

    void beforeRender (gou::Engine& engine);
    void render (gou::Renderer& renderer);

    void deselect () { m_selected_entity = entt::null; }
    entt::entity selected () const { return m_selected_entity; }
    std::string selected_name () const { return m_selected_name; }

private:
    std::vector<EntityInfo> m_entities;
    entt::entity m_selected_entity = entt::null;
    std::string m_selected_name;

    SceneAction m_scene_action = SceneAction::None;
};
