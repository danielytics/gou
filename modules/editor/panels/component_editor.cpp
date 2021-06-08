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

    if (! m_component_ptr) {
        return;
    }
    
    for (auto i = 0; i < m_component_def->attributes.size(); ++i) {
        auto& attribute = m_component_def->attributes[i];
        auto id_str = std::string{"##"} + m_component_def->name + std::string{":"} + attribute.name;
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
                break;
            case Type::UInt8:
            case Type::UInt16:
            case Type::UInt32:
            case Type::UInt64:
            case Type::Int8:
            case Type::Int16:
            case Type::Int32:
            case Type::Int64:
            case Type::Byte:
            case Type::Resource:
            case Type::TextureResource:
            case Type::MeshResource:
            case Type::Entity:
            case Type::Float:
            case Type::Double:
            case Type::Bool:
            case Type::Event:
            case Type::Ref:
            case Type::HashedString:
            case Type::RGB:
            case Type::RGBA:
            case Type::Signal:
            default: break;
        }

        if (i != m_component_def->attributes.size() - 1) {
            ImGui::TableNextRow();
        }
    }
}

void DataEditor::render () {
    const bool data_component = m_component_ptr != nullptr;
    ImGui::SetNextItemOpen(data_component, ImGuiCond_Once);
    if (ImGui::CollapsingHeader(name(), &m_group_open, data_component ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf)) {
        if (data_component) {
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
}

void DataEditor::update ()
{
    if (m_component_ptr) {
        if (m_dirty) {
            std::copy_n(m_component_copy, m_component_def->size_in_bytes, m_component_ptr);
        } else {
            std::copy_n(m_component_ptr, m_component_def->size_in_bytes, m_component_copy);
        }
        m_dirty = false;
    }
}