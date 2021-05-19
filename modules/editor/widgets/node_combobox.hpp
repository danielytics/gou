#pragma once

#include <imgui.h>

namespace widgets {

bool BeginNodeCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);
void EndNodeCombo();

}
