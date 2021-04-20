#pragma once

#include "panel.hpp"

class GlobalSettingsPanel : public Panel<GlobalSettingsPanel> {
public:
    GlobalSettingsPanel () : Panel<GlobalSettingsPanel>("Global Settings") {}
    ~GlobalSettingsPanel() {}

    void render ();
};
