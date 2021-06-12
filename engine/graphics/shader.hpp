#pragma once

#include <gou_engine.hpp>
#include <glad/glad.h>

namespace graphics {
    typedef GLint uniform_t;
    typedef GLuint buffer_t;

    struct uniform {
        GLint location;

        inline void set(float v) const {return glUniform1f(location, v);}
        inline void set(int v) const {return glUniform1i(location, v);}
        inline void set(unsigned int v) const {return glUniform1ui(location, v);}
        inline void set(const glm::vec2& v) const {return glUniform2fv(location, 1, glm::value_ptr(v));}
        inline void set(const glm::vec3& v) const {return glUniform3fv(location, 1, glm::value_ptr(v));}
        inline void set(const glm::vec4& v) const {return glUniform4fv(location, 1, glm::value_ptr(v));}
        inline void set(const glm::mat2& v) const {return glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(v));}
        inline void set(const glm::mat3& v) const {return glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(v));}
        inline void set(const glm::mat4& v) const {return glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v));}
    };

    struct Shader {
        enum class Types : GLenum {
            Fragment = GL_FRAGMENT_SHADER,
            Vertex = GL_VERTEX_SHADER,
            Geometry = GL_GEOMETRY_SHADER,
            TessControl = GL_TESS_CONTROL_SHADER,
            TessEval = GL_TESS_EVALUATION_SHADER,
        };

        void unload() const;
        void bindUnfiromBlock(const std::string& blockName, unsigned int bindingPoint) const;
        graphics::uniform uniform(const std::string& name) const;
        
        inline void use () const {
            glUseProgram(programID);
        }

        static graphics::Shader load (const spp::sparse_hash_map<Types, std::string>& shaderFiles);

        GLuint programID;
        std::array<GLuint, 5> shaders;
    };

} // graphics::
