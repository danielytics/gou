#pragma once

#include <gou/gou.hpp>
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace editors {
    inline bool f (const std::string& label, float& data) {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label.c_str());
        ImGui::TableSetColumnIndex(1);
        bool dirty = ImGui::DragFloat("##v", &data, 0.005f);
        return dirty;
    }

    inline bool vec3 (const std::string& label, float data[3]) {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label.c_str());
        ImGui::TableSetColumnIndex(1);        
        bool dirty = ImGui::DragFloat3("##f3", data, 0.005f);
        return dirty;
    }
    inline bool vec3 (const std::string& label, glm::vec3& data) {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label.c_str());
        ImGui::TableSetColumnIndex(1);
        bool dirty = ImGui::DragFloat3("##f3", (float*)(&data), 0.005f);
        return dirty;
    }

    template <typename Text> bool text (const std::string& label, Text text) {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label.c_str());
        ImGui::TableSetColumnIndex(1);
        return ImGui::InputText("##t", text.data(), text.max_size());
    }
}
