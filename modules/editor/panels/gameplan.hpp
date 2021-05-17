#pragma once

#include <imgui_node_editor.h>
#include <map>
#include <string>

#include "panel.hpp"

namespace ed = ax::NodeEditor;

enum class PinType
{
    Flow,
    Bool,
    Int,
    Float,
    Text,
    Vec2,
    Vec3,
    Vec4,
    Entity,
    Event,
};

enum class PinKind {
    Input,
    Output,
};

struct Node;

struct Pin
{
    ed::PinId id;
    std::string name;
    PinType type;
    PinKind kind;
    ::Node* node;

    Pin(const char* name, PinType type) :
        id(0), name(name), type(type), kind(PinKind::Input), node(nullptr)
    {
    }
};

struct Node
{
    ed::NodeId id;
    std::string name;
    std::vector<Pin> inputs;
    std::vector<Pin> outputs;
    ImColor color;
    ImVec2 size;

    std::string state;
    std::string savedState;

    Node(const char* name, ImColor color = ImColor(255, 255, 255)) :
        id(0), name(name), color(color), size(0, 0)
    {
    }
};

struct Link
{
    ed::LinkId id;

    ed::PinId start_pin_id;
    ed::PinId end_pin_id;

    ImColor color;

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId):
        id(id), start_pin_id(startPinId), end_pin_id(endPinId), color(255, 255, 255)
    {
    }
};

class GameplanPanel : public Panel<GameplanPanel> {
public:
    GameplanPanel () : Panel<GameplanPanel>("Gameplan Editor") {}
    ~GameplanPanel() {}

    void load ();
    void unload ();

    void render ();

private:
    std::vector<Node> m_node_templates;
    std::vector<Node> m_nodes;
    std::vector<Link> m_links;

    Pin* m_new_link_pin = nullptr;
    int m_unique_id = 1;

    Pin* findPin (ed::PinId);
    int getNextId();
};
