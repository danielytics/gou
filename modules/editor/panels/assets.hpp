#pragma once

#include "panel.hpp"

class AssetsPanel : public Panel<AssetsPanel> {
public:
    AssetsPanel () : Panel<AssetsPanel>("Assets") {}
    ~AssetsPanel() {}

    void render ();
};
