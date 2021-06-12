#include "component_editor.hpp"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <limits>

bool g_first_render = true;

namespace editors {

    bool floating (float* data) {
        bool dirty = ImGui::DragFloat("##val", data, 0.005f, 0.0f, 0.0f, "%.3f");
        return dirty;
    }

    bool vec2 (float data[2], float reset=0.0f) {
        bool dirty = false;
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize)) {
            data[0] = reset;
            dirty = true;
        }
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##X", data, 0.005f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize)) {
            data[1] = reset;
            dirty = true;
        }
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##Y", data+1, 0.005f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
        return dirty;
    }

    bool vec3 (float data[3], float reset=0.0f) {
        bool dirty = false;
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize)) {
            data[0] = reset;
            dirty = true;
        }
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##X", data, 0.005f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize)) {
            data[1] = reset;
            dirty = true;
        }
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##Y", data+1, 0.005f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize)) {
            data[2] = reset;
            dirty = true;
        }
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##Z", data+2, 0.005f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
        return dirty;
    }

    bool vec4 (float data[4], float reset=0.0f) { 
        bool dirty = false;
		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize)) {
            data[0] = reset;
            dirty = true;
        }
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##X", data, 0.005f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize)) {
            data[1] = reset;
            dirty = true;
        }
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##Y", data+1, 0.005f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize)) {
            data[2] = reset;
            dirty = true;
        }
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##Z", data+2, 0.005f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();

		// ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		// ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		// ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("W", buttonSize)) {
            data[2] = reset;
            dirty = true;
        }
		// ImGui::PopStyleColor(3);

		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##Z", data+2, 0.005f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
        return dirty;
    }

    template <typename Text> bool text (Text text) {
        return ImGui::InputText("#t", text.data(), text.max_size());
    }

    template <typename T> bool integer (const char* id, int* data, const char* format = "%d") {
        return ImGui::DragInt(id, data, 1.0f, int(std::numeric_limits<T>::min()), int(std::numeric_limits<T>::max()), format);
    }

    bool boolean (bool* data) {
        return ImGui::Checkbox("#b", data);
    }

    bool rgb (float data[3]) {
        return ImGui::ColorEdit3("#rgb", data);
    }

    bool rgba (float data[4]) {
        return ImGui::ColorEdit4("#rgba", data);
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

        // ImGui::TableSetColumnIndex(0);
        ImGui::Columns(2);
        if (g_first_render) {
            ImGui::SetColumnWidth(0, 120.0f);
            g_first_render = false;
        }
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        ImGui::NextColumn();
        // ImGui::TableSetColumnIndex(1);

        ImGui::PushID(id);

        switch (attribute.type) {
            case Type::Vec2:
                m_dirty |= editors::vec2((float*)ptr);
                break;
            case Type::Vec3:
                m_dirty |= editors::vec3((float*)ptr);
                break;
            case Type::Vec4:
                m_dirty |= editors::vec4((float*)ptr);
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
                m_dirty |= editors::floating((float*)ptr);
                break;
            case Type::Double:
                m_dirty |= editors::floating((float*)ptr);
                break;
            case Type::Bool:
                m_dirty |= editors::boolean((bool*)ptr);
                break;
            case Type::Event:
                break;
            case Type::Ref:
                break;
            case Type::HashedString:
            {
                std::array<char, 256> buffer;
                if (editors::text(buffer)) {
                    m_dirty = true;
                    *((entt::hashed_string*)ptr) = entt::hashed_string{buffer.data()};
                }
            }
            case Type::RGB:
                m_dirty |= editors::rgb((float*)ptr);
                break;
            case Type::RGBA:
                m_dirty |= editors::rgba((float*)ptr);
                break;
            case Type::Signal:
                break;
            default: break;
        }

        ImGui::PopID();

        // if (i != m_component_def->attributes.size() - 1) {
        //     ImGui::TableNextRow();
        // }
    }
}

void DataEditor::render () {
    const bool data_component = m_component_ptr != nullptr;
    ImGui::SetNextItemOpen(data_component, ImGuiCond_Once);
    if (ImGui::CollapsingHeader(name(), &m_group_open, data_component ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf)) {
        if (data_component) {
            render_editors();
            // if (ImGui::BeginTable(name(), 2, ImGuiTableFlags_None)) {
                // ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 125.0f);
                // ImGui::TableNextRow();
                // ImGui::TableSetColumnIndex(1);
                // ImGui::PushItemWidth(-FLT_MIN); 
                // render_editors();
                // ImGui::EndTable();    
            // }
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