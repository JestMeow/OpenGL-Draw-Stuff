#ifndef GL_DRAW_STUFF_RENDERER_HPP
#define GL_DRAW_STUFF_RENDERER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <GlDrawStuff/shader.hpp>
#include <GlDrawStuff/shape.hpp>
#include <GlDrawStuff/texture.hpp>

namespace GlDrawStuff {
/**
 * @brief Represents geometry stored in the renderer's buffers.
 *
 * A Mesh does not own GPU resources. It stores the information required
 * to render a previously uploaded shape using Render::draw().
 */
struct Mesh {
    GLsizei index_count;
    GLint base_vertex;
    GLintptr index_offset;
};

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

    /**
     * @brief Creates a renderer with preallocated GPU buffers.
     *
     * @param vertex_buffer_size Size in bytes of the vertex and texture coordinates buffers.
     * @param index_buffer_size Size in bytes of the index buffer.
     * @param shader Shader used when rendering shapes.
     */
    Renderer(GLsizeiptr vertex_buffer_size, GLsizeiptr index_buffer_size, Shader& shader);

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
    Mesh upload(const BaseShape& shape);

    /**
     * @brief Begins a rendering batch.
     *
     * Binds the renderer's vertex array object. Call this before
     * drawing meshes with draw().
     */
    void begin();
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
    void draw(const BaseShape& shape, const Mesh& mesh, F func);

    /**
     * @brief Renders a shape without a custom draw callback.
     *
     * @param shape Shape containing the transform, color, and optional texture.
     * @param mesh Mesh returned by upload() corresponding to the shape's geometry.
     */
    void draw(const BaseShape& shape, const Mesh& mesh);

    /**
     * @brief Ends a rendering batch.
     *
     * Unbinds the renderer's vertex array object.
     */
    void end();

    /**
     * @brief Returns the shader used by the renderer.
     *
     * @return Reference to the renderer's shader.
     */
    Shader& get_shader();
};

template<typename F>
inline void Renderer::draw(const BaseShape& shape, const Mesh& mesh, F func) {
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

}

#endif // !GL_DRAW_STUFF_RENDERER_HPP
