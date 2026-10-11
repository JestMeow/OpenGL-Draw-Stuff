#include <GlDrawStuff/shader.hpp>

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

Shader::Shader() {
        m_shader_program = glCreateProgram();
    }

    void Shader::use_default_shader() {
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

    void Shader::use() {
        glUseProgram(m_shader_program);
    }

    GLint Shader::get_uniform_location(const char* name) {
        auto it = m_uniform_locations.find(name);

        if (it != m_uniform_locations.end()) return it->second;

        GLint location = glGetUniformLocation(m_shader_program, name);
        m_uniform_locations[name] = location;

        return location;
    }

    void Shader::set_mat2(const char* name, const glm::mat2& value) {
        glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::set_mat3(const char* name, const glm::mat3& value) {
        glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::set_mat4(const char* name, const glm::mat4& value) {
        glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::set_vec2(const char* name, const glm::vec2& value) {
        glUniform2fv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void Shader::set_vec3(const char* name, const glm::vec3& value) {
        glUniform3fv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void Shader::set_vec4(const char* name, const glm::vec4& value) {
        glUniform4fv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void Shader::set_ivec2(const char* name, const glm::ivec2& value) {
        glUniform2iv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void Shader::set_ivec3(const char* name, const glm::ivec3& value) {
        glUniform3iv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void Shader::set_ivec4(const char* name, const glm::ivec4& value) {
        glUniform4iv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void Shader::set_uvec2(const char* name, const glm::uvec2& value) {
        glUniform2uiv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void Shader::set_uvec3(const char* name, const glm::uvec3& value) {
        glUniform3uiv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void Shader::set_uvec4(const char* name, const glm::uvec4& value) {
        glUniform4uiv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void Shader::set_int(const char* name, int value) {
        glUniform1i(get_uniform_location(name), value);
    }

    void Shader::set_bool(const char* name, bool value) {
        glUniform1i(get_uniform_location(name), value ? 1 : 0);
    }

    void Shader::set_float(const char* name, float value) {
        glUniform1f(get_uniform_location(name), value);
    }
};

