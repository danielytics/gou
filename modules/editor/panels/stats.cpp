
#include "stats.hpp"

void StatsPanel::render ()
{
    // auto stats = Renderer2D::GetStats();
    // ImGui::Text("Renderer2D Stats:");
    // ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    // ImGui::Text("Quads: %d", stats.QuadCount);
    // ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    // ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::Text("Current frame: %lu", current_frame);
    ImGui::Text("Current time: %.1f", current_time);
    ImGui::Text("Frame time: %.2f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);
}
