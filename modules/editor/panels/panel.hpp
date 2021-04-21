#pragma once

#include <imgui.h>
#include <gou.hpp>

namespace detail {
    HAS_MEMBER_FUNCTION(preRender,   ())
    HAS_MEMBER_FUNCTION(afterRender, ())
}

template <typename Derived>
class Panel {
public:
    Panel (const char* title, ImGuiWindowFlags window_flags=ImGuiWindowFlags_None) :
        m_title(title),
        m_window_flags(window_flags)
    {}
    ~Panel () {}

    template <typename... Args>
    void renderPanel (Args&&... args)
    {
        if constexpr (detail::hasMember_preRender<Derived>()) {
            static_cast<Derived*>(this)->preRender();
        }
        ImGui::Begin(m_title, nullptr, m_window_flags);
        static_cast<Derived*>(this)->render(std::forward<Args>(args)...);
        ImGui::End();
        if constexpr (detail::hasMember_afterRender<Derived>()) {
            static_cast<Derived*>(this)->afterRender();    
        }
    }

    /* USER FUNCTIONS:
     *
     * This class will detect and call the following methods:
     * void preRender ()        - called during render but before the panel ImGui::Begin()
     * void render (...)        - main render function, called after ImGui::Begin() and before ImGui::End()
     * void afterRender ()      - called after ImGui::End()
     * 
     * These functions all run with the render thread lock and must not modify engine data.
     * The only safe API to use is the gou::Renderer API.
     * 
     * Sometimes a panel will declare:
     *  void beforeRender (...)
     * and manually call it from Module::onBeforeRender(Engine&), this will run while holding the engine
     * lock, making it safe to call gou::Engine methods, access the EnTT registry, etc
     */

private:
    const char* m_title;
    ImGuiWindowFlags m_window_flags;
};