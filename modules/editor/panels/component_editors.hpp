#pragma once

#include "entity_properties.hpp"
#include "value_editors.hpp"
#include <typeinfo>

class DataEditor {
public:
    virtual ~DataEditor() {}
    virtual const char* name() = 0;
    inline void doRender () {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNodeEx(name(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected)) {
            render();
            ImGui::TreePop();
        }
    }
protected:
    virtual int numInputs () = 0;
    virtual void render () = 0;
};

template <typename T, int NumInputs> class TemlpatedDataEditorBase : public DataEditor {
public:
    virtual ~TemlpatedDataEditorBase() {}
    
    void update (gou::Engine& engine, T& data) {
        if (dirty) {
            beforeUpdate(engine);
            data = copy;
        } else {
            copy = data;
            afterUpdate(engine);
        }
        dirty = false;
    }
    virtual void beforeUpdate (gou::Engine&) {}
    virtual void afterUpdate (gou::Engine&) {}
protected:
    T copy;
    bool dirty = false;

private:
    int numInputs () final {
        return NumInputs;
    }
};

template <typename T> class TemlpatedDataEditor : public TemlpatedDataEditorBase<T, 0> {
public:
};

template <> class TemlpatedDataEditor<components::Position>  : public TemlpatedDataEditorBase<components::Position, 1> {
public:
    virtual ~TemlpatedDataEditor() {}
    const char* name () final { return "Position"; }
    void render () final {
        dirty |= editors::vec3("Position", (float*)(&copy));
    }
};
template <> class TemlpatedDataEditor<components::Transform>  : public TemlpatedDataEditorBase<components::Transform, 2> {
public:
    virtual ~TemlpatedDataEditor() {}
    const char* name () final { return "Transform"; }
    void render () final {
        dirty |= editors::vec3("Rotation", copy.rotation);
        dirty |= editors::vec3("Scale", copy.scale);
    }    
};
template <> class TemlpatedDataEditor<components::TimeAware>  : public TemlpatedDataEditorBase<components::TimeAware, 1> {
public:
    virtual ~TemlpatedDataEditor() {}
    const char* name () final { return "TimeAware"; }
    void render () final {
        dirty |= editors::f("Scale Factor", copy.scale_factor);
    }    
};
