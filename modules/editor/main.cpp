
#include <gou.hpp>

#include "panels/scene.hpp"
#include "panels/entity_properties.hpp"
#include "panels/global_settings.hpp"
#include "panels/assets.hpp"
#include "panels/stats.hpp"

#include <imgui_internal.h>

glm::vec4 getCentralNodeRect (ImGuiID dockspaceId)
{
    auto centeralNode = ImGui::DockBuilderGetCentralNode(dockspaceId);

    return {
        centeralNode->Pos.x,
        centeralNode->Pos.y,
        centeralNode->Size.x,
        centeralNode->Size.y
    };
}


class EditorModule : public gou::Module<EditorModule> {
    GOU_MODULE_CLASS(EditorModule)
public:
    void onLoad (gou::Engine)
    {
        m_window_flags = ImGuiWindowFlags_MenuBar
                       | ImGuiWindowFlags_NoTitleBar
                       | ImGuiWindowFlags_NoCollapse
                       | ImGuiWindowFlags_NoResize
                       | ImGuiWindowFlags_NoMove
                       | ImGuiWindowFlags_NoBringToFrontOnFocus
                       | ImGuiWindowFlags_NoBackground
                       | ImGuiWindowFlags_NoNavFocus;
    }

    void onBeforeFrame (gou::Scene& scene) {
        m_stats_panel.current_frame = scene.currentFrame();
        m_stats_panel.current_time = scene.currentTime();
    }

    void onBeforeRender (gou::Engine engine)
    {
        m_scene_panel.beforeRender(engine);
    }

    void onAfterRender (gou::Renderer& renderer)
    {
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

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N")) {
                    newScene();
                }
				if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                    openScene();
                }	
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                    saveSceneAs();
                }
#ifdef DEBUG_BUILD
				if (ImGui::MenuItem(m_show_demo ? "Hide ImGUI Demo" : "Show ImGUI Demo")) {
                    m_show_demo = !m_show_demo;
                }
#endif
				if (ImGui::MenuItem("Exit")) {
                    emit("engine/exit"_hs);
                }
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

        m_stats_panel.renderPanel();
        m_global_settings_panel.renderPanel();
        m_assets_panel.renderPanel();
        m_properties_panel.renderPanel();
        m_scene_panel.renderPanel(renderer);


#ifdef DEBUG_BUILD
        if (m_show_demo) {
            ImGui::ShowDemoWindow();
        }
#endif
#ifndef DEV_TOOLS
        noDevToolsWarning();
#endif

        renderer.setViewport(getCentralNodeRect(dockspace_id));

		ImGui::End();
    }

private:
    ImGuiWindowFlags m_window_flags;
#ifdef DEBUG_BUILD
    bool m_show_demo = false;
#endif
#ifndef DEV_TOOLS
    bool m_dev_mode_warning_open = true;
    void noDevToolsWarning () 
    {
        if (m_dev_mode_warning_open) {
            ImGui::SetNextWindowPos(ImVec2{500, 400});
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
            ImGui::Begin("WARNING", &m_dev_mode_warning_open, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking);
            ImGui::Text("You are using the editor, but not running a 'dev' build.");
            ImGui::End();
            ImGui::PopStyleColor();
        }
    }
#endif

    ScenePanel m_scene_panel;
    EntityPropertiesPanel m_properties_panel;
    AssetsPanel m_assets_panel;
    StatsPanel m_stats_panel;
    GlobalSettingsPanel m_global_settings_panel;

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
