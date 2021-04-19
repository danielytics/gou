#pragma once

#include <gou_engine.hpp>
#include <glad/glad.h>

namespace graphics::textures {

    GLuint load (const std::string& filename);
    GLuint loadArray (bool filtering, const std::vector<std::string>& filenames);

} // graphics::textures::
