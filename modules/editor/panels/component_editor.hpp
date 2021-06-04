#pragma once

#include "panel.hpp"

class DataEditor {
public:
    DataEditor (const std::string& name, const std::vector<gou::api::definitions::Attribute>& attributes, char* ptr, std::size_t size) 
        : m_name(name),
          m_attributes(attributes),
          m_component_ptr(ptr),
          m_component_size(size),
          m_dirty(false),
          m_action(EntityAction::None)
    {
        m_component_copy = new char[size];
    }

    DataEditor(DataEditor&& other)
        : m_name(other.m_name),
          m_attributes(other.m_attributes),
          m_component_ptr(other.m_component_ptr),
          m_component_size(other.m_component_size),
          m_component_copy(other.m_component_copy),
          m_dirty(other.m_dirty),
          m_action(other.m_action)
    {
        other.m_component_copy = nullptr;
    }

    ~DataEditor() {
        if (m_component_copy) {
            delete [] m_component_copy;
        }        
    }
    
    void update ();
    void render ();

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
    bool m_dirty;
    EntityAction m_action;

    const char* name () { return m_name.c_str(); }
    void render_editors ();

};
