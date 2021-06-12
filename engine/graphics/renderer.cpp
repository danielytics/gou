#include "renderer.hpp"
#include "gou_engine.hpp"

#include "mesh.hpp"

graphics::Shader g_shader;
graphics::Mesh g_mesh;
Drawable g_drawable;

GLuint matrices_ubo;

void init ()
{
    // Setup UBO for matrices
    glGenBuffers(1, &matrices_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Connect UBO to binding point 0
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, matrices_ubo, 0, 1 * sizeof(glm::mat4));

    g_shader = graphics::Shader::load({
        {graphics::Shader::Types::Vertex, "shaders/passthru.vert.glsl"},
        {graphics::Shader::Types::Fragment, "shaders/passthru.frag.glsl"},
    });

    // Connect shader UBO blocks to binding point 0
    g_shader.bindUnfiromBlock("Matrices", 0);

    g_mesh.addVertexBuffer<glm::vec3>({
            {-0.5f,  0.5f, 0.0f}, // top left
            {-0.5f, -0.5f, 0.0f}, // bottom left
            {0.5f, -0.5f, 0.0f}, // bottom right
            {0.5f,  0.5f, 0.0f},  // top right
    });
    g_mesh.addIndexBuffer({0, 1, 2, 2, 3, 0});
}


namespace constants {
    const float FULL_CIRCLE = glm::radians(360.0f);
}


void run (const glm::mat4 projection_view, std::vector<Sprite>& sprites)
{
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection_view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    g_mesh.bind();
    g_shader.use();
    for (auto& sprite : sprites) {
        auto model_matrix = glm::translate(glm::identity<glm::mat4>(), sprite.position);
        model_matrix = glm::rotate(model_matrix, constants::FULL_CIRCLE * sprite.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model_matrix = glm::rotate(model_matrix, constants::FULL_CIRCLE * sprite.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model_matrix = glm::rotate(model_matrix, constants::FULL_CIRCLE * sprite.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model_matrix = glm::scale(model_matrix, sprite.scale);

        g_shader.uniform("u_model_matrix").set(model_matrix);
        g_mesh.drawIndexed();
    }
}

void term ()
{
    g_shader.unload();
    g_mesh.unload();
    glDeleteBuffers(1, &matrices_ubo);
}
