#pragma once

#include "panel.hpp"

class GlobalSettingsPanel : public Panel<GlobalSettingsPanel> {
public:
    GlobalSettingsPanel () : Panel<GlobalSettingsPanel>("Global Settings", ImGuiWindowFlags_None, false) {}
    ~GlobalSettingsPanel() {}

    void render ();
};
