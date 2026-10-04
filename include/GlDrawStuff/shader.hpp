#ifndef GL_DRAW_STUFF_SHADER_HPP
#define GL_DRAW_STUFF_SHADER_HPP

#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace GlDrawStuff {

namespace {
constexpr const char* default_vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}";

constexpr const char* default_fragment_shader_source =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform vec4 color;\n"
    "uniform sampler2D u_Texture;\n"
    "uniform bool useTexture;\n"
    "void main() {\n"
    "    if (useTexture) {\n"
    "        FragColor = texture(u_Texture, TexCoord) * color;\n"
    "    } else {\n"
    "        FragColor = color;\n"
    "    }\n"
    "}";
}

/**
 * @brief Manages an OpenGL shader program and its uniform values.
 *
 * Shader provides an interface for creating, activating, and updating
 * an OpenGL shader program. It also caches uniform locations to avoid
 * repeatedly querying OpenGL for the same uniform.
 *
 * By defaut, the shader program is created by the constructor and can
 * be initialized with the built in vertex and fragment shaders using
 * use_default_shader().
 */
class Shader {
private:
    GLuint m_shader_program = 0;
    std::unordered_map<std::string, GLint> m_uniform_locations;

public:
    /**
     * @brief Creates an empty OpenGL shader program.
     *
     * This program must be initialized with shader stages before it can
     * be used for rendering.
     */
    Shader() {
        m_shader_program = glCreateProgram();
    }

    /**
     * @brief Compiles and link the built in default shader program.
     *
     * The default vertex shader supports position and texture coordinate
     * attributes, while the default fragment shader supports both solid
     * colors and optional texture sampling.
     *
     * @note This function does not perform shader compilation or linking
     * error checking.
     */
    void use_default_shader() {
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

        const char* vertex_shader_source = default_vertex_shader_source;
        glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        const char* fragment_shader_source = default_fragment_shader_source;
        glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);


        glAttachShader(m_shader_program, vertex_shader);
        glAttachShader(m_shader_program, fragment_shader);

        glLinkProgram(m_shader_program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    /**
     * @brief Activates this shader program for subsequent OpenGL rendering.
     *
     * Calls glUseProgram() with the shader program managed by this object.
     */
    void use() {
        glUseProgram(m_shader_program);
    }

    /**
     * @brief Gets the location of a uniform variable.
     *
     * The location is looked up with OpenGL the first time the uniform is
     * requested, and then cached for subsequent calls.
     *
     * @param name Name of the uniform variable in the shader program.
     * @return The uniform location, or -1 if the uniform does not exist
     * or is not active in the shader program.
     */
    GLint get_uniform_location(const char* name) {
        auto it = m_uniform_locations.find(name);

        if (it != m_uniform_locations.end()) return it->second;

        GLint location = glGetUniformLocation(m_shader_program, name);
        m_uniform_locations[name] = location;

        return location;
    }

    // Matrices
    /**
     * @brief Sets a 2x2 matrix uniform.
     *
     * @param name Name of uniform variable.
     * @param value Matrix value to upload.
     */
    void set_mat2(const char* name, const glm::mat2& value) {
        glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    /**
     * @brief Sets a 3x3 matrix uniform.
     *
     * @param name Name of uniform variable.
     * @param value Matrix value to upload.
     */
    void set_mat3(const char* name, const glm::mat3& value) {
        glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    /**
     * @brief Sets a 4x4 matrix uniform.
     *
     * @param name Name of uniform variable.
     * @param value Matrix value to upload.
     */
    void set_mat4(const char* name, const glm::mat4& value) {
        glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    // Vectors
    /**
     * @brief Sets a 2 component floating point vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_vec2(const char* name, const glm::vec2& value) {
        glUniform2fv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    /**
     * @brief Sets a 3 component floating point vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_vec3(const char* name, const glm::vec3& value) {
        glUniform3fv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    /**
     * @brief Sets a 4 component floating point vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_vec4(const char* name, const glm::vec4& value) {
        glUniform4fv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    /**
     * @brief Sets a 2 component signed integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_ivec2(const char* name, const glm::ivec2& value) {
        glUniform2iv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    /**
     * @brief Sets a 3 component signed integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_ivec3(const char* name, const glm::ivec3& value) {
        glUniform3iv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    /**
     * @brief Sets a 4 component signed integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_ivec4(const char* name, const glm::ivec4& value) {
        glUniform4iv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    /**
     * @brief Sets a 2 component unsigned integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_uvec2(const char* name, const glm::uvec2& value) {
        glUniform2uiv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    /**
     * @brief Sets a 3 component unsigned integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_uvec3(const char* name, const glm::uvec3& value) {
        glUniform3uiv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    /**
     * @brief Sets a 4 component unsigned integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_uvec4(const char* name, const glm::uvec4& value) {
        glUniform4uiv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    // Scalars
    /**
     * @brief Sets an integer uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Integer value to upload.
     */
    void set_int(const char* name, int value) {
        glUniform1i(get_uniform_location(name), value);
    }

    /**
     * @brief Sets a boolean uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Boolean value to upload.
     */
    void set_bool(const char* name, bool value) {
        glUniform1i(get_uniform_location(name), value ? 1 : 0);
    }

    /**
     * @brief Sets a floating point uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Floating point value to upload.
     */
    void set_float(const char* name, float value) {
        glUniform1f(get_uniform_location(name), value);
    }
};

}


#endif // !GL_DRAW_STUFF_SHADER_HPP
