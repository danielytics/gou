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
    Panel (const char* title) : m_title(title) {}
    ~Panel () {}

    template <typename... Args>
    void renderPanel (Args... args)
    {
        ImGui::Begin(m_title);
        if constexpr (detail::hasMember_beforeRender<Derived>()) {
            static_cast<Derived*>(this)->beforeRender();
        }
        static_cast<Derived*>(this)->render(args...);
        if constexpr (detail::hasMember_afterRender<Derived>()) {
            static_cast<Derived*>(this)->afterRender();    
        }
        ImGui::End();
    }

private:
    const char* m_title;
};
