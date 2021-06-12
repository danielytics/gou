#pragma once

/**
 * Define a mesh of vertices and their attributes, stored in VBO's and attached to a VAO
 * Meshes can be used for many purposes: tile maps, sprites
 */

#include <gou_engine.hpp>

#include "shader.hpp"

namespace graphics {
    namespace detail {
        template <typename T>
        struct VBOComponents {
            enum {NumComponents = 0};
        };

        template <>
        struct VBOComponents<float> {
            enum {NumComponents = 1};
        };

        template <>
        struct VBOComponents<glm::vec2> {
            enum {NumComponents = 2};
        };

        template <>
        struct VBOComponents<glm::vec3> {
            enum {NumComponents = 3};
        };

        template <>
        struct VBOComponents<glm::vec4> {
            enum {NumComponents = 4};
        };
    }

    class Mesh
    {
    public:
        enum class BufferUsage : GLenum {
            StaticDraw = GL_STATIC_DRAW,
            DynamicDraw = GL_DYNAMIC_DRAW,
            StreamDraw = GL_STREAM_DRAW,
            StaticCopy = GL_STATIC_COPY,
            DynamicCopy = GL_DYNAMIC_COPY,
            StreamCopy = GL_STREAM_COPY,
            StaticRead = GL_STATIC_READ,
            DynamicRead = GL_DYNAMIC_READ,
            StreamRead = GL_STREAM_READ,
        };

        Mesh () :
            m_count(0),
            m_num_indices(0)
        {
        }
        Mesh (Mesh&& other) :
            m_count(other.m_count),
            m_vao(other.m_vao),
            m_ebo(other.m_ebo),
            m_vbos(std::move(other.m_vbos)),
            m_num_indices(other.m_num_indices)
        {
            other.m_count = 0;
        }
            
        void unload () {
            glDeleteVertexArrays(1, &m_vao);
            for (auto vbo : m_vbos) {
                glDeleteBuffers(1, &vbo);
            }
        }
        
        inline void bind() const {
            glBindVertexArray(m_vao);
        }

        template <typename T>
        unsigned addVertexBuffer (const std::vector<T>& data, BufferUsage usage=BufferUsage::StaticDraw)
        {
            glGenVertexArrays(1, &m_vao);
            glBindVertexArray(m_vao);
            m_count = data.size();
            return addBuffer(data, usage);
        }

        template <typename T>
        unsigned addBuffer (const std::vector<T>& data, BufferUsage usage)
        {
            GLuint id = GLuint(m_vbos.size());
            buffer_t vbo;
            // Create and bind the new buffer
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            // Copy the vertex data to the buffer
            auto vertexData = reinterpret_cast<const float*>(data.data());
            glBufferData(GL_ARRAY_BUFFER, data.size() * detail::VBOComponents<T>::NumComponents * sizeof(GLfloat), vertexData, helpers::enum_value(usage));
            // Specify that the buffer data is going into attribute index 0, and contains N floats per vertex
            glVertexAttribPointer(id, detail::VBOComponents<T>::NumComponents, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(id);
            m_vbos.push_back(vbo);
            return id;
        }

        template <typename T>
        void setBuffer (unsigned id, const std::vector<T>& data, BufferUsage usage=BufferUsage::DynamicDraw) {
            buffer_t vbo = m_vbos[id];
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            // Copy the vertex data to the buffer
            auto vertex_data = reinterpret_cast<const float*>(data.data());
            glBufferData(GL_ARRAY_BUFFER, data.size() * detail::VBOComponents<T>::NumComponents * sizeof(GLfloat), vertex_data, helpers::enum_value(usage));
        }

        unsigned addIndexBuffer () {
            GLuint id = GLuint(m_vbos.size());
            glGenBuffers(1, &m_ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            m_vbos.push_back(m_ebo);
            return id;
        }
        unsigned addIndexBuffer (const std::vector<unsigned int>& indices, BufferUsage usage=BufferUsage::DynamicDraw) {
            GLuint id = GLuint(m_vbos.size());
            glGenBuffers(1, &m_ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], helpers::enum_value(usage));
            m_vbos.push_back(m_ebo);
            m_num_indices = indices.size();
            return id;
        }

        void set (unsigned id, bool enabled) {
            if (enabled) {
                glEnableVertexAttribArray(id);
            } else {
                glDisableVertexAttribArray(id);
            }
        }

        void draw () const {
            glBindVertexArray(m_vao);
            glDrawArrays(GL_TRIANGLES, 0, m_count);
        }

        void draw (unsigned int instances) const {
            glBindVertexArray(m_vao);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, m_count, instances);
        }

        void drawIndexed () const {
            glBindVertexArray(m_vao);
            glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
        }

        void drawIndexed (const std::vector<GLushort>& indices, BufferUsage usage=BufferUsage::StreamDraw) const {
            glBindVertexArray(m_vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            auto indexData = reinterpret_cast<const GLushort*>(indices.data());
            auto size = indices.size() * sizeof(GLushort);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, helpers::enum_value(usage)); // Orphan old buffer
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indexData, helpers::enum_value(usage)); // Upload new buffer
            glDrawElements(GL_TRIANGLES, GLsizei(indices.size()), GL_UNSIGNED_INT, 0);
        }

    private:
        GLsizei m_count;
        buffer_t m_vao;
        buffer_t m_ebo;
        std::vector<buffer_t> m_vbos;
        GLsizei m_num_indices;
    };

} // graphics::
