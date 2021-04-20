#pragma once

#include <imgui.h>
#include <gou.hpp>

namespace detail {
    HAS_MEMBER_FUNCTION(beforeRender, ())
    HAS_MEMBER_FUNCTION(afterRender,  ())
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
        ImGui::Begin(m_title, nullptr, m_window_flags);
        if constexpr (detail::hasMember_beforeRender<Derived>()) {
            static_cast<Derived*>(this)->beforeRender();
        }
        static_cast<Derived*>(this)->render(std::forward<Args>(args)...);
        if constexpr (detail::hasMember_afterRender<Derived>()) {
            static_cast<Derived*>(this)->afterRender();    
        }
        ImGui::End();
    }

private:
    const char* m_title;
    ImGuiWindowFlags m_window_flags;
};
