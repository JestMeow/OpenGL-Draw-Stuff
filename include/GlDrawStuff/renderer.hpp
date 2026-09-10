#ifndef GL_DRAW_STUFF_RENDERER_HPP
#define GL_DRAW_STUFF_RENDERER_HPP


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GlDrawStuff/shader.hpp>
#include <GlDrawStuff/shape.hpp>
#include <GlDrawStuff/texture.hpp>


namespace GlDrawStuff {


struct Mesh {
    GLsizei index_count;
    GLint base_vertex;
    GLintptr index_offset;
};


class Renderer {
private:
    GLintptr m_vertex_cursor = 0;
    GLintptr m_texcoord_cursor = 0;
    GLintptr m_index_cursor = 0;
    
    Shader& m_shader;

public:
    GLuint VBO = 0;
    GLuint TEXCOORD_VBO = 0;
    GLuint VAO = 0;
    GLuint EBO = 0;


    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, -1.f, 1.f);


    Renderer(GLsizeiptr vertex_buffer_size, GLsizeiptr index_buffer_size, Shader& shader): m_shader(shader) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &TEXCOORD_VBO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertex_buffer_size,
            nullptr,
            GL_DYNAMIC_DRAW
        );

        // Pos

        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec3),
            nullptr
        );

        glEnableVertexAttribArray(0);

        // Tex
        glBindBuffer(GL_ARRAY_BUFFER, TEXCOORD_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec2),
            nullptr
        );

        glEnableVertexAttribArray(1);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            index_buffer_size,
            nullptr,
            GL_DYNAMIC_DRAW
        );

        glBindVertexArray(0);
    }


    Mesh upload(const BaseShape& shape) {
        Geometry geometry = shape.geometry();

        Mesh mesh{};

        mesh.index_count = static_cast<GLsizei>(geometry.index_count);

        mesh.base_vertex = static_cast<GLint>(m_vertex_cursor / sizeof(glm::vec3));

        mesh.index_offset = m_index_cursor;



        // Upload vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferSubData(
            GL_ARRAY_BUFFER,
            m_vertex_cursor,
            geometry.vertex_count * sizeof(glm::vec3),
            geometry.vertices
        );

        // Upload texture coords
        glBindBuffer(GL_ARRAY_BUFFER, TEXCOORD_VBO);

        glBufferSubData(
            GL_ARRAY_BUFFER,
            m_texcoord_cursor,
            geometry.vertex_count * sizeof(glm::vec2),
            geometry.texture_coordinates
        );
        
        // Upload indices        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBufferSubData(
            GL_ELEMENT_ARRAY_BUFFER,
            m_index_cursor,
            geometry.index_count * sizeof(unsigned int),
            geometry.indices
        );


        m_vertex_cursor += geometry.vertex_count * sizeof(glm::vec3);
        m_texcoord_cursor += geometry.vertex_count * sizeof(glm::vec2);
        m_index_cursor += geometry.index_count * sizeof(unsigned int);

        return mesh;
    }


    void begin() {
        glBindVertexArray(VAO);
    }


    void draw(const BaseShape& shape, const Mesh& mesh) {
        m_shader.use();

        glm::mat4 model{1.f};

        model = glm::translate(model, shape.position);

        model = glm::rotate(
            model,
            shape.orientation.x,
            glm::vec3(1, 0, 0)
        );

        model = glm::rotate(
            model,
            shape.orientation.y,
            glm::vec3(0, 1, 0)
        );
    
        model = glm::rotate(
            model,
            shape.orientation.z,
            glm::vec3(0, 0, 1)
        );

        model = glm::scale(model, shape.scale);



        if (shape.get_texture() != nullptr) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shape.get_texture()->texture);

            m_shader.set_int("u_Texture", 0);
            m_shader.set_int("useTexture", 1);
        } else {
            m_shader.set_int("useTexture", 0);
        }


        m_shader.set_mat4("model", model);
        m_shader.set_mat4("view", view);
        m_shader.set_mat4("projection", projection);
        
        m_shader.set_vec4("color", shape.color);

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            mesh.index_count,
            GL_UNSIGNED_INT,
            reinterpret_cast<void*>(mesh.index_offset),
            mesh.base_vertex
        );
    }

    void end() {
        glBindVertexArray(0);
    }
};


}


#endif // !GL_DRAW_STUFF_RENDERER_HPP
