
#include "render_api.hpp"

graphics::RenderAPI::~RenderAPI()
{
    // Clean up OpenGL and the window
    spdlog::info("Deleting graphics context.");
    SDL_GL_DeleteContext(gl_render_context);
    SDL_GL_DeleteContext(gl_init_context);
    SDL_DestroyWindow(window);
}

// These methods should only be called by the render thread

void graphics::RenderAPI::setViewport (const glm::vec4& rect)
{
    if (m_rect != rect) {
        m_rect = rect;
        const int height = entt::monostate<"graphics/resolution/height"_hs>();
        m_viewport = {
            // Bottom left corner of viewable area
            rect.x, height - (rect.y + rect.w),
            // Size of viewable area
            rect.z, rect.w,
        };
        updateProjectionMatrix();
        dirty.store(true);
    }
}

void graphics::RenderAPI::updateProjectionMatrix ()
{
    const float field_of_view = entt::monostate<"graphics/renderer/field-of-view"_hs>();
    const float near_distance = entt::monostate<"graphics/renderer/near-distance"_hs>();
    const float far_distance = entt::monostate<"graphics/renderer/far-distance"_hs>();

    m_projection_matrix = glm::perspective(glm::radians(field_of_view), m_viewport.z / m_viewport.w, near_distance, far_distance);
    // m_projection_matrix = glm::ortho(m_viewport.x, m_viewport.x + m_viewport.z, m_viewport.y + m_viewport.w, m_viewport.y, near_distance, far_distance);
}

// This method can be called form the engine threads
void graphics::RenderAPI::windowChanged ()
{
    const int width = entt::monostate<"graphics/resolution/width"_hs>();
    const int height = entt::monostate<"graphics/resolution/height"_hs>();
    m_viewport = glm::vec4(0, 0, width, height);
    m_rect = m_viewport;
    updateProjectionMatrix();
}
