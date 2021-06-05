#pragma once

#include "panel.hpp"

class DataEditor {
public:
    DataEditor (const gou::api::definitions::Component& component_def, char* ptr) 
        : m_component_def(component_def),
          m_component_ptr(ptr),
          m_dirty(false),
          m_action(EntityAction::None)
    {
        m_component_copy = new char[component_def.size_in_bytes];
    }

    DataEditor(DataEditor&& other)
        : m_component_def(other.m_component_def),
          m_component_ptr(other.m_component_ptr),
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
    
    void remove (gou::Engine& engine, entt::entity entity) {
        m_component_def.manage(engine.engine.registry(gou::api::Registry::Runtime), entity, gou::api::definitions::ManageOperation::Remove);
    }

private:
    const gou::api::definitions::Component& m_component_def;
    char* m_component_ptr;
    char* m_component_copy;
    bool m_dirty;
    EntityAction m_action;

    const char* name () { return m_component_def.name.c_str(); }
    void render_editors ();

};
