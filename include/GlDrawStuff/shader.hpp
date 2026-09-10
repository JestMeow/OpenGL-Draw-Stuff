#ifndef GL_DRAW_STUFF_SHADER_HPP
#define GL_DRAW_STUFF_SHADER_HPP


#include <string>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


namespace GlDrawStuff {


namespace {
    std::string default_vertex_shader_source =
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

    std::string default_fragment_shader_source =
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


class Shader {
public:
    GLuint shader_program = 0;

    Shader() {
        shader_program = glCreateProgram();
    }

    void use_default_shader() {
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

        const char* vertex_shader_source = default_vertex_shader_source.c_str();
        glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        const char* fragment_shader_source = default_fragment_shader_source.c_str();
        glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);


        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);

        glLinkProgram(shader_program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    void use() {
        glUseProgram(shader_program);
    }

    void set_mat4(const char* name, const glm::mat4& value) {
        GLint location = glGetUniformLocation(shader_program, name);

        glUniformMatrix4fv(
            location,
            1,
            GL_FALSE,
            glm::value_ptr(value)
        );
    }

    void set_vec4(const char* name, const glm::vec4& value) {
        GLint location = glGetUniformLocation(shader_program, name);

        glUniform4fv(
            location,
            1,
            glm::value_ptr(value)
        );
    }

    void set_int(const char* name, int value) {
        GLint location = glGetUniformLocation(shader_program, name);

        glUniform1i(
            location,
            value
        );
    }
};

}


#endif // !GL_DRAW_STUFF_SHADER_HPP
