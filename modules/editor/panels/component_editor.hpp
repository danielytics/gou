#pragma once

#include "panel.hpp"

class DataEditor {
public:
    DataEditor (const gou::api::definitions::Component& component_def, char* ptr) 
        : m_component_def(&component_def),
          m_component_ptr(ptr),
          m_dirty(false),
          m_component_copy(nullptr)
    {
        if (ptr) {
            m_component_copy = new char[component_def.size_in_bytes];
        }
    }

    DataEditor(DataEditor&& other)
        : m_component_def(other.m_component_def),
          m_component_ptr(other.m_component_ptr),
          m_component_copy(other.m_component_copy),
          m_dirty(other.m_dirty)
    {
        other.m_component_copy = nullptr;
    }

    void operator= (DataEditor&& other)
    {
        m_component_def = other.m_component_def;
        m_component_ptr = other.m_component_ptr;
        m_component_copy = other.m_component_copy;
        m_dirty = other.m_dirty;
        other.m_component_copy = nullptr;
    }

    ~DataEditor() {
        if (m_component_copy) {
            delete [] m_component_copy;
        }        
    }
    
    void update ();
    void render ();

    bool removed () { return !m_group_open; }
    const gou::api::definitions::Component& component_def () { return *m_component_def; };

private:
    const gou::api::definitions::Component* m_component_def;
    char* m_component_ptr;
    char* m_component_copy;
    bool m_dirty;
    bool m_group_open = true;

    const char* name () { return m_component_def->name.c_str(); }
    void render_editors ();

};
