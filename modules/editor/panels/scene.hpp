#pragma once

#include "panel.hpp"

class ScenePanel : public Panel<ScenePanel> {
public:
    ScenePanel () : Panel<ScenePanel>("Scene") {}
    ~ScenePanel() {}

    void render (gou::Renderer& renderer);
};
