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

private:
    std::vector<EntityInfo> entities;
};
