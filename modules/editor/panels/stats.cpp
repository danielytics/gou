
#include "stats.hpp"

void StatsPanel::render ()
{
    // auto stats = Renderer2D::GetStats();
    // ImGui::Text("Renderer2D Stats:");
    // ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    // ImGui::Text("Quads: %d", stats.QuadCount);
    // ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    // ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::Text("Current frame: %lu Current time: %.1f", current_frame, current_time);
    ImGui::Text("Average frame time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);
}
