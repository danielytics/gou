#pragma once

#include "render_api.hpp"

#ifndef WITHOUT_IMGUI

#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

namespace imgui {
    void initTheme ();
}

ImGuiContext* createImGuiContext()
{
    IMGUI_CHECKVERSION();
    return ImGui::CreateContext();
}

void initImGui (graphics::RenderAPI* render_api)
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable multiple windows
    imgui::initTheme();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(render_api->window, render_api->gl_render_context);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void handleImGuiEvents (core::Engine& engine)
{
    for (const auto& event : engine.inputEvents()) {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }
}

void newImGuiFrame (graphics::RenderAPI* render_api)
{
    EASY_BLOCK("New ImGui Frame", profiler::colors::Orange200);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(render_api->window);
    ImGui::NewFrame();
}

void endImGuiFrame (graphics::RenderAPI* render_api)
{
    EASY_BLOCK("Rendering ImGui", profiler::colors::Orange200);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(render_api->window, render_api->gl_render_context);
    }
}

void destroyImGuiContext ()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

#else // No ImGui

ImGuiContext* createImGuiContext() { return nullptr; }
void initImGui (graphics::RenderAPI*) {}
void handleImGuiEvents (core::Engine&) {}
void newImGuiFrame (graphics::RenderAPI*) {}
void endImGuiFrame (graphics::RenderAPI*) {}
void destroyImGuiContext () {}

#endif