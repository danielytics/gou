#include "component_editor.hpp"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <limits>

namespace editors {

    bool floating (const char* id, float* data) {
        bool dirty = ImGui::DragFloat(id, data, 0.005f);
        return dirty;
    }

    bool vec2 (const char* id, float data[2]) {
        bool dirty = ImGui::DragFloat2(id, data, 0.005f);
        return dirty;
    }

    bool vec3 (const char* id, float data[3]) {    
        bool dirty = ImGui::DragFloat3(id, data, 0.005f);
        return dirty;
    }

    bool vec4 (const char* id, float data[4]) { 
        bool dirty = ImGui::DragFloat4(id, data, 0.005f);
        return dirty;
    }

    template <typename Text> bool text (const char* id, Text text) {
        return ImGui::InputText(id, text.data(), text.max_size());
    }

    template <typename T> bool integer (const char* id, int* data, const char* format = "%d") {
        return ImGui::DragInt(id, data, 1.0f, int(std::numeric_limits<T>::min()), int(std::numeric_limits<T>::max()), format);
    }

    bool boolean (const char* id, bool* data) {
        return ImGui::Checkbox(id, data);
    }

    bool rgb (const char* id, float data[3]) {
        return ImGui::ColorEdit3(id, data);
    }

    bool rgba (const char* id, float data[4]) {
        return ImGui::ColorEdit4(id, data);
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

        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        ImGui::TableSetColumnIndex(1);

        switch (attribute.type) {
            case Type::Vec2:
                m_dirty |= editors::vec2(id, (float*)ptr);
                break;
            case Type::Vec3:
                m_dirty |= editors::vec3(id, (float*)ptr);
                break;
            case Type::Vec4:
                m_dirty |= editors::vec4(id, (float*)ptr);
                break;
            case Type::UInt8:
                m_dirty |= editors::integer<std::uint8_t>(id, (int*)ptr);
                break;
            case Type::UInt16:
                m_dirty |= editors::integer<std::uint16_t>(id, (int*)ptr);
                break;
            case Type::UInt32:
                m_dirty |= editors::integer<std::uint32_t>(id, (int*)ptr);
                break;
            case Type::Int8:
                m_dirty |= editors::integer<std::int8_t>(id, (int*)ptr);
                break;
            case Type::Int16:
                m_dirty |= editors::integer<std::int16_t>(id, (int*)ptr);
                break;
            case Type::Int32:
                m_dirty |= editors::integer<std::int32_t>(id, (int*)ptr);
            case Type::Byte:
                m_dirty |= editors::integer<std::byte>(id, (int*)ptr, "%x");
            case Type::Resource:
                break;
            case Type::TextureResource:
                break;
            case Type::MeshResource:
                break;
            case Type::Entity:
                break;
            case Type::Float:
                m_dirty |= editors::floating(id, (float*)ptr);
                break;
            case Type::Double:
                m_dirty |= editors::floating(id, (float*)ptr);
                break;
            case Type::Bool:
                m_dirty |= editors::boolean(id, (bool*)ptr);
                break;
            case Type::Event:
                break;
            case Type::Ref:
                break;
            case Type::HashedString:
            {
                std::array<char, 256> buffer;
                if (editors::text(id, buffer)) {
                    m_dirty = true;
                    *((entt::hashed_string*)ptr) = entt::hashed_string{buffer.data()};
                }
            }
            case Type::RGB:
                m_dirty |= editors::rgb(id, (float*)ptr);
                break;
            case Type::RGBA:
                m_dirty |= editors::rgba(id, (float*)ptr);
                break;
            case Type::Signal:
                break;
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
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 125.0f);
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