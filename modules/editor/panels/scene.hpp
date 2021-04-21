#pragma once

#include "panel.hpp"

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

    entt::entity selected () const { return m_selected_entity; }

private:
    std::vector<EntityInfo> m_entities;
    entt::entity m_selected_entity = entt::null;
};
