
#include <gou.hpp>
#include <imgui.h>

class EditorModule : public gou::Module<EditorModule> {
    GOU_MODULE_CLASS(EditorModule)
public:
    void onLoad (gou::Engine) {
        m_window_flags = ImGuiWindowFlags_MenuBar
                       | ImGuiWindowFlags_NoTitleBar
                       | ImGuiWindowFlags_NoCollapse
                       | ImGuiWindowFlags_NoResize
                       | ImGuiWindowFlags_NoMove
                       | ImGuiWindowFlags_NoBringToFrontOnFocus
                       | ImGuiWindowFlags_NoNavFocus;
    }

    void onBeforeFrame (gou::Scene& scene) {
        current_frame = scene.currentFrame();
        current_time = scene.currentTime();
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
		ImGui::Begin("DockSpace Demo", nullptr, m_window_flags);
		ImGui::PopStyleVar(3);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 200.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		}
		style.WindowMinSize.x = minWinSizeX;

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

				if (ImGui::MenuItem("Exit")) {
                    emit("engine/exit"_hs);
                }
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Stats");

		// auto stats = Renderer2D::GetStats();
		// ImGui::Text("Renderer2D Stats:");
		// ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		// ImGui::Text("Quads: %d", stats.QuadCount);
		// ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		// ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

        renderer.setViewport({
            viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y,
            viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y,
        });

		ImGui::End();
		ImGui::PopStyleVar();

        ImGui::ShowDemoWindow();

		ImGui::End();
    }

private:
    std::uint64_t current_frame;
    Time current_time;
    ImGuiWindowFlags m_window_flags;

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
