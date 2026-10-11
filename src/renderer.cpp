#include <GlDrawStuff/renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GlDrawStuff {
Renderer::Renderer(GLsizeiptr vertex_buffer_size, GLsizeiptr index_buffer_size, Shader& shader): m_shader(shader) {
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

Mesh Renderer::upload(const BaseShape& shape) {
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

void Renderer::begin() {
    glBindVertexArray(VAO);
}

void Renderer::draw(const BaseShape& shape, const Mesh& mesh) {
    draw(shape, mesh, [](){});
}

void Renderer::end() {
    glBindVertexArray(0);
}

Shader& Renderer::get_shader() {
    return m_shader;
}

};

