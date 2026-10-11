#ifndef GL_DRAW_STUFF_SHADER_HPP
#define GL_DRAW_STUFF_SHADER_HPP

#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace GlDrawStuff {
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
    Shader();

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
    void use_default_shader();

    /**
     * @brief Activates this shader program for subsequent OpenGL rendering.
     *
     * Calls glUseProgram() with the shader program managed by this object.
     */
    void use();

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
    GLint get_uniform_location(const char* name);

    // Matrices
    /**
     * @brief Sets a 2x2 matrix uniform.
     *
     * @param name Name of uniform variable.
     * @param value Matrix value to upload.
     */
    void set_mat2(const char* name, const glm::mat2& value);

    /**
     * @brief Sets a 3x3 matrix uniform.
     *
     * @param name Name of uniform variable.
     * @param value Matrix value to upload.
     */
    void set_mat3(const char* name, const glm::mat3& value);

    /**
     * @brief Sets a 4x4 matrix uniform.
     *
     * @param name Name of uniform variable.
     * @param value Matrix value to upload.
     */
    void set_mat4(const char* name, const glm::mat4& value);

    // Vectors
    /**
     * @brief Sets a 2 component floating point vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_vec2(const char* name, const glm::vec2& value);

    /**
     * @brief Sets a 3 component floating point vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_vec3(const char* name, const glm::vec3& value);

    /**
     * @brief Sets a 4 component floating point vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_vec4(const char* name, const glm::vec4& value);

    /**
     * @brief Sets a 2 component signed integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_ivec2(const char* name, const glm::ivec2& value);

    /**
     * @brief Sets a 3 component signed integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_ivec3(const char* name, const glm::ivec3& value);

    /**
     * @brief Sets a 4 component signed integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_ivec4(const char* name, const glm::ivec4& value);

    /**
     * @brief Sets a 2 component unsigned integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_uvec2(const char* name, const glm::uvec2& value);

    /**
     * @brief Sets a 3 component unsigned integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_uvec3(const char* name, const glm::uvec3& value);

    /**
     * @brief Sets a 4 component unsigned integer vector uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Vector value to upload.
     */
    void set_uvec4(const char* name, const glm::uvec4& value);

    // Scalars
    /**
     * @brief Sets an integer uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Integer value to upload.
     */
    void set_int(const char* name, int value);

    /**
     * @brief Sets a boolean uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Boolean value to upload.
     */
    void set_bool(const char* name, bool value);

    /**
     * @brief Sets a floating point uniform.
     *
     * @param name Name of the uniform variable.
     * @param value Floating point value to upload.
     */
    void set_float(const char* name, float value);
};

}

#endif // !GL_DRAW_STUFF_SHADER_HPP
