#include "gameplan.hpp"
#include "../widgets/icons.hpp"
#include "../widgets/node_combobox.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

static ed::EditorContext* g_Context = nullptr;

const std::map<std::string, PinType> g_pin_types = {
    {"flow",   PinType::Flow},
    {"bool",   PinType::Bool},
    {"num",    PinType::Num},
    {"text",   PinType::Text},
    {"vec2",   PinType::Vec2},
    {"vec3",   PinType::Vec3},
    {"vec4",   PinType::Vec4},
    {"entity", PinType::Entity},
    {"event",  PinType::Event},
    {"any",    PinType::Any},
};
const std::map<PinType, ImColor> g_pin_type_colors = {
    {PinType::Flow,   ImColor(147, 226,  74)},
    {PinType::Bool,   ImColor(220,  48,  48)},
    {PinType::Num,    ImColor( 68, 201, 156)},
    {PinType::Text,   ImColor(124,  21, 153)},
    {PinType::Vec2,   ImColor( 51, 150, 215)},
    {PinType::Vec3,   ImColor(218,   0, 183)},
    {PinType::Vec4,   ImColor(255,  48,  48)},
    {PinType::Entity, ImColor( 48,  48, 255)},
    {PinType::Event,  ImColor( 48, 255, 255)},
    {PinType::Any,    ImColor(255, 255, 255)},
};
const std::map<PinType, ax::Widgets::IconType> g_pin_type_icons = {
    {PinType::Flow,   ax::Widgets::IconType::Flow},
    {PinType::Bool,   ax::Widgets::IconType::Diamond},
    {PinType::Num,    ax::Widgets::IconType::Circle},
    {PinType::Text,   ax::Widgets::IconType::Circle},
    {PinType::Vec2,   ax::Widgets::IconType::Circle},
    {PinType::Vec3,   ax::Widgets::IconType::Circle},
    {PinType::Vec4,   ax::Widgets::IconType::Circle},
    {PinType::Entity, ax::Widgets::IconType::Circle},
    {PinType::Event,  ax::Widgets::IconType::Circle},
    {PinType::Any,    ax::Widgets::IconType::Grid},
};

void DrawPinIcon(const Pin& pin, bool connected, int alpha)
{
    using ax::Widgets::IconType;
    IconType icon_type = g_pin_type_icons.at(pin.type);
    ImColor  color = g_pin_type_colors.at(pin.type);
    color.Value.w = alpha / 255.0f;
    ax::Widgets::Icon(ImVec2(12, 12), icon_type, connected, color, ImColor(32, 32, 32, alpha));
};

static bool canCreateLink(Pin* a, Pin* b)
{
    return a && b // a and b exist
        && a != b // a and b are not the same pin
        && a->kind != b->kind // one is input, the other is output
        && (a->type == b->type || a->type == PinType::Any || b->type == PinType::Any) // types match
        && a->node != b->node; // nodes can't connect to themselves
}

// ImFont* AddDefaultFont( float pixel_size )
// {
//     ImGuiIO& io = ImGui::GetIO();
//     ImFontConfig config;
//     config.SizePixels = pixel_size;
//     config.OversampleH = config.OversampleV = 1;
//     config.PixelSnapH = true;
//     ImFont* font = io.Fonts->AddFontDefault(&config);
//     return font;
// }

int GameplanPanel::getNextId()
{
    return m_unique_id++;
}

void GameplanPanel::load ()
{
    g_Context = ed::CreateEditor();
    {
        m_node_templates.emplace_back("Event", ImColor(255, 0, 0));
        m_node_templates.back().outputs.emplace_back("Entity", PinType::Entity);
    }
    {
        m_node_templates.emplace_back("Current Entity", ImColor(255, 0, 0));
        m_node_templates.back().outputs.emplace_back("Entity", PinType::Entity);
    }
    {
        m_node_templates.emplace_back("=", ImColor(255, 0, 0));
        m_node_templates.back().inputs.emplace_back("A", PinType::Any);
        m_node_templates.back().inputs.emplace_back("B", PinType::Any);
        m_node_templates.back().outputs.emplace_back("True", PinType::Bool);
        m_node_templates.back().outputs.emplace_back("False", PinType::Bool);
    }
    {
        m_node_templates.emplace_back("Compare", ImColor(255, 0, 0));
        m_node_templates.back().inputs.emplace_back("A", PinType::Num);
        m_node_templates.back().inputs.emplace_back("B", PinType::Num);
        m_node_templates.back().outputs.emplace_back("True", PinType::Bool);
        m_node_templates.back().outputs.emplace_back("False", PinType::Bool);
        m_node_templates.back().operations.push_back(">");
        m_node_templates.back().operations.push_back(">=");
        m_node_templates.back().operations.push_back("<");
        m_node_templates.back().operations.push_back("<=");
    }
    {
        m_node_templates.emplace_back("Boolean", ImColor(255, 0, 0));
        m_node_templates.back().operations.push_back("True");
        m_node_templates.back().operations.push_back("False");
        m_node_templates.back().outputs.emplace_back("Value", PinType::Bool);
    }
    {
        m_node_templates.emplace_back("Send Value", ImColor(255, 0, 0));
        m_node_templates.back().inputs.emplace_back("Trigger", PinType::Bool);
        m_node_templates.back().outputs.emplace_back("Value", PinType::Any);
    }

    for (auto& node : m_node_templates) {
        for (auto& input : node.inputs) {
            input.kind = PinKind::Input;
        }
        for (auto& output : node.outputs) {
            output.kind = PinKind::Output;
        }
    }
}

void GameplanPanel::unload ()
{
    ed::DestroyEditor(g_Context);
}

void GameplanPanel::render ()
{
    ed::SetCurrentEditor(g_Context);
    ed::Begin("Gameplan");

    const char* hover_text = nullptr;

    // Render nodes
    for (auto& node : m_nodes) {
        ed::PushStyleColor(ed::StyleColor_NodeBg,        ImColor(229, 229, 229, 200));
        ed::PushStyleColor(ed::StyleColor_NodeBorder,    ImColor(125, 125, 125, 200));
        ed::PushStyleColor(ed::StyleColor_PinRect,       ImColor(255, 229, 229, 160));
        ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(125, 125, 125, 160));

        ed::PushStyleVar(ed::StyleVar_NodePadding,  ImVec4(3, 0, 3, 0));
        ed::PushStyleVar(ed::StyleVar_NodeRounding, 0.0f);
        ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f,  1.0f));
        ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
        ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
        ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
        ed::PushStyleVar(ed::StyleVar_PinRadius, 6.0f);

        ed::BeginNode(node.id);
        if (! node.inputs.empty()) {
            int index = 0;
            int count = node.inputs.size();
            for (auto& input : node.inputs) {
                bool last = ++index == count;

                auto alpha = ImGui::GetStyle().Alpha;
                if (m_new_link_pin && !canCreateLink(m_new_link_pin, &input) && &input != m_new_link_pin) {
                    alpha = alpha * (48.0f / 255.0f);
                }

                //ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
                //ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
                //ed::PushStyleVar(ed::StyleVar_PinCorners, 15);
                ed::BeginPin(input.id, ed::PinKind::Input);
                DrawPinIcon(input, /* is pin linked? */ false, (int)(alpha * 255));
                ed::EndPin();
                if (ImGui::IsItemHovered()) {
                    hover_text = input.name.c_str();
                }
                if (! last) {
                    ImGui::SameLine();
                }
                
                //ed::PopStyleVar(3);
                // ed::PopStyleVar(1);
            }
        }

        if (node.operations.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Text("%s", node.name.c_str());
            ImGui::PopStyleColor();
        } else {
            ImGui::SetNextItemWidth(50.0f);
            if (widgets::BeginNodeCombo((std::string{"##"} + node.name).c_str(), node.operations[node.selected].c_str(), ImGuiComboFlags_None))
            {
                for (int i = 0; i < node.operations.size(); ++i) {
                    const bool is_selected = node.selected == i;
                    if (ImGui::Selectable(node.operations[i].c_str(), is_selected)) {
                        node.selected = i;
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                widgets::EndNodeCombo();
            }
        }
        
        if (! node.outputs.empty()) {
            int index = 0;
            int count = node.inputs.size();
            for (auto& output : node.outputs) {
                bool last = ++index == count;

                auto alpha = ImGui::GetStyle().Alpha;
                if (m_new_link_pin && !canCreateLink(m_new_link_pin, &output) && &output != m_new_link_pin) {
                    alpha = alpha * (48.0f / 255.0f);
                }

                //ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
                //ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
                //ed::PushStyleVar(ed::StyleVar_PinCorners, 15);
                ed::BeginPin(output.id, ed::PinKind::Output);
                DrawPinIcon(output, /* is pin linked? */ false, (int)(alpha * 255));
                if (ImGui::IsItemHovered()) {
                    hover_text = output.name.c_str();
                }
                ed::EndPin();
                if (! last) {
                    ImGui::SameLine();
                }
                
                //ed::PopStyleVar(3);
                // ed::PopStyleVar(1);
            }
        }
        ed::EndNode();
        ed::PopStyleVar(7);
        ed::PopStyleColor(4);
    }

    // Render links
    for (auto link : m_links) {
        ed::Link(link.id, link.start_pin_id, link.end_pin_id, link.color);
    }

    if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
        auto showLabel = [](const char* label, ImColor color)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
            auto size = ImGui::CalcTextSize(label);

            auto padding = ImGui::GetStyle().FramePadding;
            auto spacing = ImGui::GetStyle().ItemSpacing;

            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

            auto rectMin = ImGui::GetCursorScreenPos() - padding;
            auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

            auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
            ImGui::TextUnformatted(label);
        };

        ed::PinId start_pin_id = 0, end_pin_id = 0;
        if (ed::QueryNewLink(&start_pin_id, &end_pin_id))
        {
            auto start_pin = findPin(start_pin_id);
            auto end_pin   = findPin(end_pin_id);

            m_new_link_pin = start_pin ? start_pin : end_pin;

            if (start_pin->kind == PinKind::Input)
            {
                std::swap(start_pin, end_pin);
                std::swap(start_pin_id, end_pin_id);
            }

            if (start_pin && end_pin)
            {
                if (start_pin == end_pin)
                {
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
                else if (start_pin->kind == end_pin->kind)
                {
                    showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
                //else if (endPin->Node == startPin->Node)
                //{
                //    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                //    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                //}
                else if (start_pin->type != end_pin->type && start_pin->type != PinType::Any && end_pin->type != PinType::Any)
                {
                    showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                }
                else
                {
                    showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                    if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                    {
                        m_links.emplace_back(Link(getNextId(), start_pin_id, end_pin_id));
                        m_links.back().color = g_pin_type_colors.at(start_pin->type);
                    }
                }
            }
        }

        // ed::PinId pinId = 0;
        // if (ed::QueryNewNode(&pinId))
        // {
        //     newLinkPin = FindPin(pinId);
        //     if (newLinkPin)
        //         showLabel("+ Create Node", ImColor(32, 45, 32, 180));

        //     if (ed::AcceptNewItem())
        //     {
        //         createNewNode  = true;
        //         newNodeLinkPin = FindPin(pinId);
        //         newLinkPin = nullptr;
        //         ed::Suspend();
        //         ImGui::OpenPopup("Create New Node");
        //         ed::Resume();
        //     }
        // }
    } else {
        m_new_link_pin = nullptr;
    }

    ed::EndCreate();

    // if (ed::BeginDelete())
    // {
    //     ed::LinkId linkId = 0;
    //     while (ed::QueryDeletedLink(&linkId))
    //     {
    //         if (ed::AcceptDeletedItem())
    //         {
    //             auto id = std::find_if(s_Links.begin(), s_Links.end(), [linkId](auto& link) { return link.ID == linkId; });
    //             if (id != s_Links.end())
    //                 s_Links.erase(id);
    //         }
    //     }

    //     ed::NodeId nodeId = 0;
    //     while (ed::QueryDeletedNode(&nodeId))
    //     {
    //         if (ed::AcceptDeletedItem())
    //         {
    //             auto id = std::find_if(s_Nodes.begin(), s_Nodes.end(), [nodeId](auto& node) { return node.ID == nodeId; });
    //             if (id != s_Nodes.end())
    //                 s_Nodes.erase(id);
    //         }
    //     }
    // }
    // ed::EndDelete();

    // ImGui::SetCursorScreenPos(cursorTopLeft);

    // Context menus
    auto open_popup_position = ImGui::GetMousePos();
    ed::Suspend();
    ed::NodeId context_node_id;
    ed::PinId context_pin_id;
    ed::LinkId context_link_id;
    if (ed::ShowNodeContextMenu(&context_node_id)) {
        ImGui::OpenPopup("Node Context Menu");
    } else if (ed::ShowPinContextMenu(&context_pin_id)) {
        ImGui::OpenPopup("Pin Context Menu");
    } else if (ed::ShowLinkContextMenu(&context_link_id)) {
        ImGui::OpenPopup("Link Context Menu");
    } else if (ed::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("Create New Node");
    }
    ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Node Context Menu")) {
        // auto node = FindNode(contextNodeId);

        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();
        // if (node)
        // {
        //     ImGui::Text("ID: %p", node->ID.AsPointer());
        //     ImGui::Text("Type: %s", node->Type == NodeType::Blueprint ? "Blueprint" : (node->Type == NodeType::Tree ? "Tree" : "Comment"));
        //     ImGui::Text("Inputs: %d", (int)node->Inputs.size());
        //     ImGui::Text("Outputs: %d", (int)node->Outputs.size());
        // }
        // else
        //     ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
        // ImGui::Separator();
        // if (ImGui::MenuItem("Delete"))
        //     ed::DeleteNode(contextNodeId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Pin Context Menu"))
    {
        // auto pin = FindPin(contextPinId);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        // if (pin)
        // {
        //     ImGui::Text("ID: %p", pin->ID.AsPointer());
        //     if (pin->Node)
        //         ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
        //     else
        //         ImGui::Text("Nodte: %s", "<none>");
        // }
        // else
        //     ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Link Context Menu"))
    {
        // auto link = FindLink(contextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        // if (link)
        // {
        //     ImGui::Text("ID: %p", link->ID.AsPointer());
        //     ImGui::Text("From: %p", link->StartPinID.AsPointer());
        //     ImGui::Text("To: %p", link->EndPinID.AsPointer());
        // }
        // else
        //     ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
        // ImGui::Separator();
        // if (ImGui::MenuItem("Delete"))
        //     ed::DeleteLink(contextLinkId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Create New Node")) {
        for (auto template_node : m_node_templates) {
            if (ImGui::MenuItem(template_node.name.c_str())) {
                m_nodes.push_back(template_node);
                auto& node = m_nodes.back();
                node.id = getNextId();
                for (auto& input : node.inputs) {
                    input.id = getNextId();
                }
                for (auto& output : node.outputs) {
                    output.id = getNextId();
                }
                ed::SetNodePosition(node.id, open_popup_position);
                break;
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
    ed::Resume();

    ed::End();

    if (hover_text) {
        ImGui::SetTooltip("%s", hover_text);
    }
}

Pin* GameplanPanel::findPin (ed::PinId pin_id)
{
    for (auto& node : m_nodes) {
        for (auto& pin : node.inputs) {
            if (pin.id == pin_id) {
                return &pin;
            }
        }
        for (auto& pin : node.outputs) {
            if (pin.id == pin_id) {
                return &pin;
            }
        }
    }
    return nullptr;
}
