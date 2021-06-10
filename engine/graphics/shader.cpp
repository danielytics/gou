
#include "shader.hpp"

GLuint compileAndAttach (GLuint shaderProgram, GLenum shaderType, const std::string& filename, const std::string& shaderSource)
{
    GLuint shader = glCreateShader(shaderType);

    // Compile the shader
    char* source = const_cast<char*>(shaderSource.c_str());
    int32_t size = int32_t(shaderSource.length());
    glShaderSource(shader, 1, &source, &size);
    glCompileShader(shader);

    // Check for compile errors
    int wasCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &wasCompiled);
    if (wasCompiled == 0)
    {
        // Find length of shader info log
        int maxLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // Get shader info log
        char* shaderInfoLog = new char[maxLength];
        glGetShaderInfoLog(shader, maxLength, &maxLength, shaderInfoLog );

        spdlog::critical("Failed to compile shader: {}\n{}", filename, shaderInfoLog);

        delete [] shaderInfoLog;

        // Signal error
        return GLuint(-1);
    }

    // Attach the compiled program
    glAttachShader(shaderProgram, shader);
    return shader;
}

graphics::Shader graphics::Shader::load (const spp::sparse_hash_map<graphics::Shader::Types, std::string>& shaderFiles)
{
    GLuint shaderProgram = glCreateProgram();
    std::vector<GLuint> shaders;
    for (auto [type, filename] : shaderFiles) {
        auto shaderType = helpers::enum_value(type);
        auto source = helpers::readToString(filename);
        GLuint shader = compileAndAttach(shaderProgram, shaderType, filename, source);
        shaders.push_back(shader);
    }
    // Link the shader programs into one
    glLinkProgram(shaderProgram);
    int isLinked;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, reinterpret_cast<int*>(&isLinked));
    if (!isLinked) {
        // Find length of shader info log
        int maxLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // Get shader info log
        char* shaderProgramInfoLog = new char[maxLength];
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, shaderProgramInfoLog);

        delete [] shaderProgramInfoLog;
        spdlog::critical("Linking shaders failed.\n{}", shaderProgramInfoLog);
    }
    return {shaderProgram, shaders};
}


void graphics::Shader::unload () const
{
    glUseProgram(0);
    for (auto shader : shaders) {
        glDetachShader(programID, shader);
        glDeleteShader(shader);
    }
    glDeleteProgram(programID);
}

void graphics::Shader::bindUnfiromBlock(const std::string& blockName, unsigned int bindingPoint) const
{
    GLuint location = glGetUniformBlockIndex(programID, blockName.c_str());
    glUniformBlockBinding(programID, location, bindingPoint);
}

graphics::uniform graphics::Shader::uniform(const std::string& name) const
{
    return {
        glGetUniformLocation(programID, name.c_str())
    };
}
