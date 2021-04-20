
#include "viewport.hpp"

void ViewportPanel::beforeRender ()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
}

void ViewportPanel::render (gou::Renderer& renderer)
{
    auto viewport_min_region = ImGui::GetWindowContentRegionMin();
    auto viewport_max_region = ImGui::GetWindowContentRegionMax();
    auto viewport_offset = ImGui::GetWindowPos();

    renderer.setViewport({
        viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y,
        viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y,
    });
}

void ViewportPanel::afterRender ()
{
    ImGui::PopStyleVar();
}
