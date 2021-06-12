#pragma once

#include <gou_engine.hpp>

#include "shader.hpp"

struct Sprite {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

void init ();
void run (const glm::mat4 projection_matrix, std::vector<Sprite>& sprites);
void term ();
