
#include "render_api.hpp"

graphics::RenderAPI::~RenderAPI()
{
    // Clean up OpenGL and the window
    spdlog::info("Deleting graphics context.");
    SDL_GL_DeleteContext(gl_render_context);
    SDL_GL_DeleteContext(gl_init_context);
    SDL_DestroyWindow(window);
}

void graphics::RenderAPI::setViewport (const glm::vec4& viewport)
{
    if (m_viewport != viewport) {
        m_viewport = viewport;
        dirty = true;
    }
}

// This method can be called form the engine threads
void graphics::RenderAPI::windowChanged ()
{
    const float field_of_view = entt::monostate<"graphics/renderer/field-of-view"_hs>();
    const float near_distance = entt::monostate<"graphics/renderer/near-distance"_hs>();
    const float far_distance = entt::monostate<"graphics/renderer/far-distance"_hs>();
    const float width = entt::monostate<"graphics/renderer/width"_hs>();
    const float height = entt::monostate<"graphics/renderer/height"_hs>();

    m_projection_matrix = glm::perspective(glm::radians(field_of_view), float(width) / float(height), near_distance, far_distance);
    m_viewport = glm::vec4(0, 0, int(width), int(height));
    dirty = true;
    resolution_changed = true;
}
