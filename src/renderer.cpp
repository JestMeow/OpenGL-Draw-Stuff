#include <GlDrawStuff/renderer.h>

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GlDrawStuff/shader.hpp>
#include <GlDrawStuff/shape.hpp>
#include <GlDrawStuff/texture.hpp>



/**
 * @brief Manages OpenGL buffers and renders uploaded shapes.
 *
 * Renderer maintains shared vertex, texture coordinates, and index buffers.
 * Shapes can be uploaded using upload(), which returns a Mesh describing
 * the uploaded geometry. The returned Mesh can then be passed
 * to render the shape.
 *
 * @note The renderer does NOT own the Shader passed to its constructor.
 */
class RendererImpl {
private:
    GLintptr m_vertex_cursor = 0;
    GLintptr m_texcoord_cursor = 0;
    GLintptr m_index_cursor = 0;
    
    GlDrawStuff::Shader& m_shader;

public:
    GLuint VBO = 0;
    GLuint TEXCOORD_VBO = 0;
    GLuint VAO = 0;
    GLuint EBO = 0;

    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, -1.f, 1.f);

    /**
     * @brief Creates a renderer with preallocated GPU buffers.
     *
     * @param vertex_buffer_size Size in bytes of the vertex and texture coordinates buffers.
     * @param index_buffer_size Size in bytes of the index buffer.
     * @param shader Shader used when rendering shapes.
     */
    RendererImpl(GLsizeiptr vertex_buffer_size, GLsizeiptr index_buffer_size, GlDrawStuff::Shader& shader): m_shader(shader) {
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

    ~RendererImpl() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &TEXCOORD_VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

    /**
     * @brief Upload a shape's geometry to the renderer's GPU buffers.
     *
     * The geometry is appended to the existing contents of the renderer's
     * buffers. The returned Mesh contains the offsets and counts required
     * to render the uploaded geometry.
     *
     * @param shape Shape whose geometry should be uploaded
     * @return A Mesh describing the uploaded geometry.
     *
     * @warning The renderer's buffers must have sufficient remaining capacity
     * for the shape's vertices, texture coordinates, and indices.
     */
    Mesh upload(const GlDrawStuff::BaseShape& shape) {
        GlDrawStuff::Geometry geometry = shape.geometry();

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

    /**
     * @brief Begins a rendering batch.
     *
     * Binds the renderer's vertex array object. Call this before
     * drawing meshes with draw().
     */
    void begin() {
        glBindVertexArray(VAO);
    }
    
    /**
     * @brief Renders an uploaded shape.
     *
     * Condigures the model, view, and projection matrices, binds the shape's
     * texture when present, sets the shape's color, invokes the supplied
     * callback, and issues the OpenGL draw call.
     *
     * @tparam F Callable type of the callback invoked immediately before
     * the geometry is drawn
     *
     * @param shape Shape containing the transform, color, and optional texture.
     * @param mesh Mesh returned by upload() corresponding to the shape's geometry.
     * @param func Callback invoked after render state has been configured and
     * immediately before the draw call.
     */
    template<typename F>
    void draw(const GlDrawStuff::BaseShape& shape, const Mesh& mesh, F func) {
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

        func();

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            mesh.index_count,
            GL_UNSIGNED_INT,
            reinterpret_cast<void*>(mesh.index_offset),
            mesh.base_vertex
        );
    }

    /**
     * @brief Renders a shape without a custom draw callback.
     *
     * @param shape Shape containing the transform, color, and optional texture.
     * @param mesh Mesh returned by upload() corresponding to the shape's geometry.
     */
    void draw(const GlDrawStuff::BaseShape& shape, const Mesh& mesh) {
        draw(shape, mesh, [](){});
    }

    /**
     * @brief Ends a rendering batch.
     *
     * Unbinds the renderer's vertex array object.
     */
    void end() {
        glBindVertexArray(0);
    }

    /**
     * @brief Returns the shader used by the renderer.
     *
     * @return Reference to the renderer's shader.
     */
    GlDrawStuff::Shader& get_shader() {
        return m_shader;
    }
};




struct Renderer {
    RendererImpl* impl;
};

extern "C" {

Renderer* renderer_create(GLsizeiptr vertex_buffer_size, GLsizeiptr index_buffer_size, void* shader) {
    if (!shader) return nullptr;

    try {
        auto impl = std::make_unique<RendererImpl>(vertex_buffer_size, index_buffer_size, *static_cast<GlDrawStuff::Shader*>(shader));

        auto renderer = std::make_unique<Renderer>();

        renderer->impl = impl.release();

        return renderer.release();
    }
    catch (...) {
        return nullptr;
    }
}

void renderer_destroy(Renderer *renderer) {
    if (!renderer) return;

    delete renderer->impl;
    delete renderer;
}

Mesh renderer_upload(Renderer *renderer, void *shape) {
    if (!renderer || !shape) return {};
    return renderer->impl->upload(*static_cast<GlDrawStuff::BaseShape*>(shape)); 
}

void renderer_begin(Renderer* renderer) {
    if (!renderer) return;
    renderer->impl->begin();
}

void renderer_draw(Renderer *renderer, void *shape, Mesh mesh) {
    if (!renderer || !shape) return;
    renderer->impl->draw(*static_cast<GlDrawStuff::BaseShape*>(shape), mesh);
}

void renderer_end(Renderer *renderer) {
    if (!renderer) return;
    renderer->impl->end();
}

}


