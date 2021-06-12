
#include <gou/gou.hpp>

#include "panels/scene.hpp"
#include "panels/entity_properties.hpp"
#include "panels/global_settings.hpp"
#include "panels/assets.hpp"
#include "panels/stats.hpp"
#include "panels/gameplan.hpp"

#include "widgets/curve_editor.hpp"

#include <IconsForkAwesome.h>
#include <imgui_internal.h>

glm::vec4 getCentralNodeRect (ImGuiID dockspaceId)
{
    auto centeralNode = ImGui::DockBuilderGetCentralNode(dockspaceId);
    return {
        centeralNode->Pos.x,
        centeralNode->Pos.y,
        centeralNode->Size.x,
        centeralNode->Size.y,
    };
}

template <typename WidgetFunc>
void maybeDisabled(bool disabled, const std::array<float, 3>& rgb, WidgetFunc widget_func)
{
    if (disabled) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(rgb[0], rgb[1], rgb[2], 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(rgb[0], rgb[1], rgb[2], 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(rgb[0], rgb[1], rgb[2], 1.0f));
    }
    widget_func();
    ImGui::PopStyleColor(3);
    if (disabled) {
        ImGui::PopItemFlag();
    }
}

static std::map<gou::types::Type, std::string> type_to_string {
    {gou::types::Type::Vec2,            "vec2"},
    {gou::types::Type::Vec3,            "vec3"},
    {gou::types::Type::Vec4,            "vec4"},
    {gou::types::Type::UInt8,           "uint8"},
    {gou::types::Type::UInt16,          "uint16"},
    {gou::types::Type::UInt32,          "uint32"},
    {gou::types::Type::UInt64,          "uint64"},
    {gou::types::Type::Int8,            "int8"},
    {gou::types::Type::Int16,           "int16"},
    {gou::types::Type::Int32,           "int32"},
    {gou::types::Type::Int64,           "int64"},
    {gou::types::Type::Byte,            "byte"},
    {gou::types::Type::Resource,        "resource"},
    {gou::types::Type::TextureResource, "texture-resource"},
    {gou::types::Type::MeshResource,    "mesh-resource"},
    {gou::types::Type::Entity,          "entity"},
    {gou::types::Type::Float,           "float"},
    {gou::types::Type::Double,          "double"},
    {gou::types::Type::Bool,            "bool"},
    {gou::types::Type::Event,           "event"},
    {gou::types::Type::Ref,             "ref"},
    {gou::types::Type::HashedString,    "hashed-string"},
    {gou::types::Type::RGB,             "rgb"},
    {gou::types::Type::RGBA,            "rgba"},
    {gou::types::Type::Signal,          "signal"},
};


class EditorModule : public gou::Module<EditorModule> {
    GOU_MODULE_CLASS(EditorModule)
public:
    void onLoad (gou::Engine engine)
    {
        m_window_flags = ImGuiWindowFlags_MenuBar
                       | ImGuiWindowFlags_NoTitleBar
                       | ImGuiWindowFlags_NoCollapse
                       | ImGuiWindowFlags_NoResize
                       | ImGuiWindowFlags_NoMove
                       | ImGuiWindowFlags_NoBringToFrontOnFocus
                       | ImGuiWindowFlags_NoBackground
                       | ImGuiWindowFlags_NoNavFocus;
        
        m_gameplan_panel.load();

        // Load components
        m_properties_panel.load(engine);

        // Load fonts
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        engine.readDataFile("fonts/" FONT_ICON_FILE_NAME_FK, m_font_buffer);
        if (! m_font_buffer.empty()) {
            io.Fonts->AddFontFromMemoryTTF(m_font_buffer.data(), m_font_buffer.size(), 16.0f, &icons_config, m_icons_ranges);
        }
        io.Fonts->Build();

    }

    void onUnload (gou::Engine)
    {
        m_gameplan_panel.unload();
        // Unload icon font
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        m_font_buffer.clear();
    }

    void onBeforeFrame (gou::Scene& scene) {
        m_stats_panel.current_frame = scene.currentFrame();
        m_stats_panel.current_time = scene.currentTime();
    }

    void onPrepareRender (gou::Engine engine)
    {
        if (m_exit) {
            engine.emit("engine/exit"_hs);
        }
        m_scene_panel.beforeRender(engine);
        if (m_scene_panel.selected() != m_properties_panel.selected()) {
            m_properties_panel.select(m_scene_panel.selected(), m_scene_panel.selected_name());
        }
        m_properties_panel.beforeRender(engine, engine.scene);
        if (m_scene_panel.selected() != entt::null && m_properties_panel.selected() == entt::null) {
            m_scene_panel.deselect();
        }
    }

    void onAfterRender (gou::Renderer& renderer)
    { 
        if (! m_setup) {
            ImGui::GetStyle().TabBorderSize = 1.0f;
            m_setup = true;
        }
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", nullptr, m_window_flags);
		ImGui::PopStyleVar(3);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float min_win_size_x = style.WindowMinSize.x;
		style.WindowMinSize.x = 200.0f;
        ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode);
		style.WindowMinSize.x = min_win_size_x;

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New", "Ctrl+N")) {
                    newScene();
                }
				if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                    openScene();
                }	
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                    saveSceneAs();
                }
				if (ImGui::MenuItem("Exit")) {
                    m_exit = true;
                }
				ImGui::EndMenu();
			}

            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem(m_scene_panel.visible() ? "Hide Scene" : "Show Scene", "Ctrl+1")) {
                    m_scene_panel.toggleVisible();
                }
                if (ImGui::MenuItem(m_properties_panel.visible() ? "Hide Entity Properties" : "Show Entity Properties", "Ctrl+2")) {
                    m_properties_panel.toggleVisible();
                }
                if (ImGui::MenuItem(m_gameplan_panel.visible() ? "Hide Gameplan" : "Show Gameplan", "Ctrl+3")) {
                    m_gameplan_panel.toggleVisible();
                }
                if (ImGui::MenuItem(m_assets_panel.visible() ? "Hide Assets" : "Show Assets", "Ctrl+4")) {
                    m_assets_panel.toggleVisible();
                }
                if (ImGui::MenuItem(m_global_settings_panel.visible() ? "Hide Global Settings" : "Show Global Settings", "Ctrl+5")) {
                    m_global_settings_panel.toggleVisible();
                }
                if (ImGui::MenuItem(m_stats_panel.visible() ? "Hide Stats" : "Show Stats", "Ctrl+6")) {
                    m_stats_panel.toggleVisible();
                }
#ifdef DEBUG_BUILD
				if (ImGui::MenuItem(m_show_curve_editor ? "Hide Curve Editor" : "Show Curve Editor")) {
                    m_show_curve_editor = !m_show_curve_editor;
                }
				if (ImGui::MenuItem(m_show_demo ? "Hide ImGUI Demo" : "Show ImGUI Demo")) {
                    m_show_demo = !m_show_demo;
                }
#endif
                ImGui::EndMenu();
            }

			ImGui::EndMenuBar();
		}


        bool running = m_running;
        ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        maybeDisabled(running, {0.1f, 0.7f, 0.1f}, [this](){
            if (ImGui::Button(ICON_FK_PLAY)) {
                emit("scene/registry/runtime->background"_event);
                emit("engine/set-system-status/running"_event);
                m_running = true;
            }
        });
        ImGui::SameLine();
        maybeDisabled(!running, {0.7f, 0.1f, 0.1f}, [this](){
            if (ImGui::Button(ICON_FK_STOP)) {
                emit("engine/set-system-status/stopped"_event);
                emit("scene/registry/background->runtime"_event);
                emit("scene/registry/clear-background"_event);
                m_properties_panel.reset();
                m_running = false;
            }
        });
        ImGui::End();

        m_scene_panel.renderPanel(renderer);
        m_properties_panel.renderPanel();
        m_gameplan_panel.renderPanel();
        m_assets_panel.renderPanel();
        m_global_settings_panel.renderPanel();
        m_stats_panel.renderPanel();

#ifdef DEBUG_BUILD
        if (m_show_curve_editor) {
            ImGui::Begin("Bezier Curve Editor");
            ImGui::ShowBezierDemo();
            ImGui::End();
        }
        if (m_show_demo) {
            ImGui::ShowDemoWindow();
        }
#endif

        renderer.setViewport(getCentralNodeRect(dockspace_id));

		ImGui::End();
    }

private:
    ImGuiWindowFlags m_window_flags;
#ifdef DEBUG_BUILD
    bool m_show_demo = false;
    bool m_show_curve_editor = false;
#endif

    ScenePanel m_scene_panel;
    EntityPropertiesPanel m_properties_panel;
    AssetsPanel m_assets_panel;
    StatsPanel m_stats_panel;
    GlobalSettingsPanel m_global_settings_panel;
    GameplanPanel m_gameplan_panel;

    bool m_exit = false;
    bool m_running = false;
    bool m_setup = false;

    static constexpr ImWchar m_icons_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
    std::string m_font_buffer;

    void newScene ()
    {

    }

    void openScene ()
    {

    }

    void saveSceneAs ()
    {

    }
};

GOU_MODULE(EditorModule)
