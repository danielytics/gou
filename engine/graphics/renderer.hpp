#pragma once

#include <gou_engine.hpp>

#include "shader.hpp"

struct Sprite {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

struct Texture {
    enum class Type {
        Albedo,
    };

    unsigned int id;
    Type type;
};

struct Drawable {
    struct Texture {
        unsigned int id;
        graphics::uniform uniform;
    };
    graphics::Shader m_shader;
    std::vector<Texture> m_textures;
};

class Mesh {
    public:
        // mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh ();
        Mesh (std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

        Drawable drawable (graphics::Shader& shader);
        void draw (const Drawable& drawable);

        void unload ();

    private:
        //  render data
        unsigned int vao, vbo, ebo;

        void setupMesh();
};  

void init ();
void run (const glm::mat4 projection_matrix, std::vector<Sprite>& sprites);
void term ();