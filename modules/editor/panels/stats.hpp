#pragma once

#include "panel.hpp"

class StatsPanel : public Panel<StatsPanel> {
public:
    StatsPanel () : Panel<StatsPanel>("Stats") {}
    ~StatsPanel() {}

    void render ();

    std::uint64_t current_frame;
    Time current_time;
};
