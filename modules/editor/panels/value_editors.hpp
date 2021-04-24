#pragma once

#include <gou/gou.hpp>
#include <imgui.h>

namespace editors {
    inline bool f (const std::string& label, float& data) {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label.c_str());
        ImGui::SameLine();
        return ImGui::InputFloat((std::string{"##"} + label).c_str(), &data, 0.01f, 1.0f, "%.3f");
    }

    inline bool vec3 (const std::string& label, float data[3]) {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label.c_str());
        ImGui::SameLine();
        return ImGui::InputFloat3((std::string{"##"} + label).c_str(), data);
    }
    inline bool vec3 (const std::string& label, glm::vec3& data) {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label.c_str());
        ImGui::SameLine();
        return ImGui::InputFloat3((std::string{"##"} + label).c_str(), (float*)(&data));
    }

    template <typename Text> bool text (const std::string& label, Text text) {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label.c_str());
        ImGui::SameLine();
        return ImGui::InputText((std::string{"##"} + label).c_str(), text.data(), text.max_size());
    }
}
