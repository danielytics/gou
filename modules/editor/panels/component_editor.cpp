#include "component_editor.hpp"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace editors {

    inline bool f (const char* id, const char* label, float& data) {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        ImGui::TableSetColumnIndex(1);
        bool dirty = ImGui::DragFloat(id, &data, 0.005f);
        return dirty;
    }

    inline bool vec2 (const char* id, const char* label, float data[2]) {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        ImGui::TableSetColumnIndex(1);        
        bool dirty = ImGui::DragFloat2(id, data, 0.005f);
        return dirty;
    }
    inline bool vec3 (const char* id, const char* label, float data[3]) {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        ImGui::TableSetColumnIndex(1);        
        bool dirty = ImGui::DragFloat3(id, data, 0.005f);
        return dirty;
    }
    inline bool vec4 (const char* id, const char* label, float data[4]) {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        ImGui::TableSetColumnIndex(1);        
        bool dirty = ImGui::DragFloat4(id, data, 0.005f);
        return dirty;
    }

    template <typename Text> bool text (const char* id, const char* label, Text text) {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        ImGui::TableSetColumnIndex(1);
        return ImGui::InputText(id, text.data(), text.max_size());
    }

}

void DataEditor::render_editors () {
    using Type = gou::types::Type;
    for (auto i = 0; i < m_attributes.size(); ++i) {
        auto& attribute = m_attributes[i];
        auto id_str = std::string{"##"} + m_name + std::string{":"} + attribute.name;
        const char* id = id_str.c_str();
        char* ptr = m_component_copy + attribute.offset;
        char label[attribute.name.size() + 1];
        std::copy(attribute.name.begin(), attribute.name.end(), label);
        label[0] = std::toupper(label[0]); // Capitalise the string
        label[attribute.name.size()] = 0; // NULL terminate the string

        switch (attribute.type) {
            case Type::Vec2:
                m_dirty |= editors::vec2(id, label, (float*)ptr);
                break;
            case Type::Vec3:
                m_dirty |= editors::vec3(id, label, (float*)ptr);
                break;
            case Type::Vec4:
                m_dirty |= editors::vec4(id, label, (float*)ptr);
            default:
            break;
        }
        
        if (i != m_attributes.size() - 1) {
            ImGui::TableNextRow();
        }
    }
}

void DataEditor::render () {
    m_action = EntityAction::None;
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::CollapsingHeader(name(), ImGuiTreeNodeFlags_None)) {
        if (ImGui::BeginPopupContextWindow(name(), ImGuiMouseButton_Right, false)) {
            if (ImGui::MenuItem("Remove Component")) {
                m_action = EntityAction::RemoveComponent;
            }
            ImGui::EndPopup();
        }
        
        if (ImGui::BeginTable(name(), 2, ImGuiTableFlags_None)) {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(-FLT_MIN); 
            render_editors();
            ImGui::EndTable();    
        }
    }
}

void DataEditor::update () {
    if (m_dirty) {
        std::copy_n(m_component_copy, m_component_size, m_component_ptr);
    } else {
        std::copy_n(m_component_ptr, m_component_size, m_component_copy);
    }
    m_dirty = false;
}