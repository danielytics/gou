#include "renderer.hpp"
#include "gou_engine.hpp"

Mesh::Mesh ()
{
    vao = GLuint(-1);
    vbo = GLuint(-1);
    ebo = GLuint(-1);
}

Mesh::Mesh (std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setupMesh ()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
  
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

    glBindVertexArray(0);
}

Drawable Mesh::drawable (graphics::Shader& shader) 
{
    Drawable drawable {shader};
    unsigned int albedo_num = 1;
    for(unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string name;
        switch (textures[i].type) {
            case Texture::Type::Albedo:
                name = "material.albedo" + std::to_string(albedo_num++);
                break;
        };
        drawable.m_textures.push_back({textures[i].id, shader.uniform(name)});
    }
    return drawable;
}  

void Mesh::draw (const Drawable& drawable)
{
    EASY_FUNCTION(profiler::colors::Orange300)

    // Bind textures
    unsigned i = 0;
    for(auto& [texture_id, uniform] : drawable.m_textures) {
        glActiveTexture(GL_TEXTURE0 + i++); // activate proper texture unit before binding
        uniform.set(float(i));
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    glActiveTexture(GL_TEXTURE0);

    // Draw mesh
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::unload ()
{
    if (vao != GLuint(-1)) {
        glDeleteVertexArrays(1, &vao);
    }
    if (vbo != GLuint(-1)) {
        glDeleteBuffers(1, &vbo);
    }
    if (ebo != GLuint(-1)) {
        glDeleteBuffers(1, &ebo);
    }
}

graphics::Shader g_shader;
Mesh g_mesh;
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

    // g_mesh = Mesh(
    //     {
    //         {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // top left
    //         {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // bottom left
    //         {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // bottom right
    //         {{0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // top right
    //     },
    //     {0, 1, 2, 2, 3, 0},
    //     {}
    // );
    g_mesh = Mesh(
        {
            {{-0.5f,  0.5f, 0.0f}, {}, {}}, // top left
            {{-0.5f, -0.5f, 0.0f}, {}, {}}, // bottom left
            {{0.5f, -0.5f, 0.0f},  {}, {}}, // bottom right
            {{0.5f,  0.5f, 0.0f},  {}, {}},  // top right
        },
        {0, 1, 2, 2, 3, 0},
        {}
    );

    g_drawable = g_mesh.drawable(g_shader);
}


namespace constants {
    const float FULL_CIRCLE = glm::radians(360.0f);
}


void run (const glm::mat4 projection_view, std::vector<Sprite>& sprites)
{
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection_view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    g_shader.use();
    g_shader.uniform("u_projection_view").set(projection_view);
    for (auto& sprite : sprites) {
        auto model_matrix = glm::translate(glm::identity<glm::mat4>(), sprite.position);
        model_matrix = glm::rotate(model_matrix, constants::FULL_CIRCLE * sprite.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model_matrix = glm::rotate(model_matrix, constants::FULL_CIRCLE * sprite.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model_matrix = glm::rotate(model_matrix, constants::FULL_CIRCLE * sprite.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model_matrix = glm::scale(model_matrix, sprite.scale);

        g_shader.uniform("u_model").set(model_matrix);
        g_mesh.draw(g_drawable);
    }
}

void term ()
{
    g_shader.unload();
    g_mesh.unload();
    glDeleteBuffers(1, &matrices_ubo);
}
