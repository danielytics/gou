#pragma once

#include "value_editors.hpp"
#include <typeinfo>

class DataEditor {
public:
    DataEditor (const std::string& name, const std::vector<gou::api::definitions::Attribute>& attributes, char* ptr, std::size_t size) 
        : m_name(name),
          m_attributes(attributes),
          m_component_ptr(ptr),
          m_component_size(size)
    {
        // m_component_copy = new char[size];
    }
    ~DataEditor() {
        // delete [] m_component_copy;
    }
    
    void update () {
        if (dirty) {
            std::copy_n(m_component_copy, m_component_size, m_component_ptr);
        } else {
            std::copy_n(m_component_ptr, m_component_size, m_component_copy);
        }
        dirty = false;
    }

    inline void render () {
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

    EntityAction action () const { return m_action; }
    
    void remove (gou::Scene& scene, entt::entity entity) {
        // scene.remove<T>(entity);
    }

private:
    const std::string& m_name;
    const std::vector<gou::api::definitions::Attribute>& m_attributes;
    char* m_component_ptr;
    std::size_t m_component_size;
    char* m_component_copy;
    bool dirty = false;
    EntityAction m_action;

    const char* name () { return m_name.c_str(); }

    void render_editors () {
        // dirty |= editors::vec3("Position", (float*)(&copy));
    }

};


// template <typename T, int NumInputs> class TemlpatedDataEditorBase : public DataEditor {
// public:
//     virtual ~TemlpatedDataEditorBase(const std::string& name)
//         : m_name(name),
//     {}
    
//     void update (gou::Engine& engine, T& data) {
//         if (dirty) {
//             beforeUpdate(engine);
//             data = copy;
//         } else {
//             copy = data;
//             afterUpdate(engine);
//         }
//         dirty = false;
//     }
//     virtual void beforeUpdate (gou::Engine&) {}
//     virtual void afterUpdate (gou::Engine&) {}

// protected:
//     T copy;
//     bool dirty = false;

// private:
//     void remove (gou::Scene& scene, entt::entity entity) final {
//         scene.remove<T>(entity);
//     }
// };

// template <typename T> class TemlpatedDataEditor : public TemlpatedDataEditorBase<T, 0> {
// public:
// };

// template <> class TemlpatedDataEditor<components::Position>  : public TemlpatedDataEditorBase<components::Position, 1> {
// public:
//     virtual ~TemlpatedDataEditor() {}
//     const char* name () final { return "Position"; }
//     void render () final {
//         dirty |= editors::vec3("Position", (float*)(&copy));
//     }
// };
// template <> class TemlpatedDataEditor<components::Transform>  : public TemlpatedDataEditorBase<components::Transform, 2> {
// public:
//     virtual ~TemlpatedDataEditor() {}
//     const char* name () final { return "Transform"; }
//     void render () final {
//         dirty |= editors::vec3("Rotation", copy.rotation);
//         ImGui::TableNextRow();
//         dirty |= editors::vec3("Scale", copy.scale);
//     }    
// };

// class DataEditorWidgets {
// public:
//     virtual void begin (const std::string& name) = 0;
//     virtual bool end () = 0;

//     // Basic scalar values
//     virtual void scalar_float (const std::string& label, float& value) = 0;
//     virtual void scalar_int (const std::string& label, int& value) = 0;
//     virtual void scalar_uint (const std::string& label, unsigned& value) = 0;
//     virtual void scalar_byte (const std::string& label, std::byte& value) = 0;
//     virtual void scalar_bool (const std::string& label, bool& value) = 0;

//     // Vector values
//     virtual void vec2 (const std::string& label, glm::vec2& value) = 0;
//     virtual void vec3 (const std::string& label, glm::vec3& value) = 0;
//     virtual void vec4 (const std::string& label, glm::vec4& value) = 0;

//     // Colors
//     virtual void rgb  (const std::string& label, glm::vec3& value) = 0;
//     virtual void rgba (const std::string& label, glm::vec4& value) = 0;

//     // Entities, signals and resources
//     virtual void entity (const std::string& label, entt::entity& value, entt::entity self=entt::null) = 0;
//     virtual void signal (const std::string& label, std::uint32_t& value) = 0;
// };

// void foo (gou::Engine& engine) {
//     auto func = [](DataEditorWidgets* editors, entt::entity entity, components::TriggerRegion& component){
//         editors->begin("TriggerRegion");
//         editors->signal("On Enter", component.on_enter);
//         editors->signal("On Exit", component.on_exit);
//         return editors->end();
//     };
// }
