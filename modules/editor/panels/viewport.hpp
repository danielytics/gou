#pragma once

#include "panel.hpp"

class ViewportPanel : public Panel<ViewportPanel> {
public:
    ViewportPanel () : Panel<ViewportPanel>("Viewport") {}
    ~ViewportPanel() {}

    void beforeRender ();
    void render (gou::Renderer& renderer);
    void afterRender ();
};
