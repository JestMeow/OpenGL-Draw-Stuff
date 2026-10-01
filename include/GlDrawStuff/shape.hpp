#ifndef GL_DRAW_STUFF_SHAPE_HPP
#define GL_DRAW_STUFF_SHAPE_HPP


#include <memory>

#include <array>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GlDrawStuff/shader.hpp>
#include <GlDrawStuff/color.hpp>
#include <GlDrawStuff/texture.hpp>


namespace GlDrawStuff {


struct Geometry{
    const glm::vec3* vertices;
    GLsizei vertex_count;

    const glm::vec2* texture_coordinates;
    GLsizei texture_coordinate_count;

    const unsigned int* indices;
    GLsizei index_count;
};

class BaseShape {
private:
    // NON-OWNING. Must outlive this shape object.
    Shader* m_shader = nullptr;
    // NON-OWNING. Must outlive this shape object.
    Texture* m_texture = nullptr;

public:
    glm::vec4 color{1.f};

    glm::vec3 position{0.f};
    glm::vec3 orientation{0.f};
    glm::vec3 scale{1.f};

    

    virtual ~BaseShape() = default;

    virtual Geometry geometry() const = 0;


    void set_shader(Shader& s) & {
        m_shader = &s;
    }
    void set_shader(Shader& s) && = delete;


    void set_texture(Texture& t) & {
        m_texture = &t;
    }
    void set_texture(Texture& t) && = delete;


    void set_color(glm::vec4 c) {
        color = c;
    }

    Texture* get_texture() const {
        return m_texture;
    }
};


namespace Shape {
    class Triangle : public BaseShape {
    public:
        std::array<glm::vec3, 3> vertices;
        std::array<glm::vec2, 3> texture_coordinate {
            glm::vec2{1.f, 1.f},
            glm::vec2{1.f, 0.f},
            glm::vec2{0.f, 0.f}
        };
        std::array<unsigned int, 3> indices{0, 1, 2};

        static Triangle make(std::array<glm::vec3, 3> verts, glm::vec3 pos = {0.f, 0.f, 0.f}) {
            Triangle t;
            t.vertices = verts;
            t.position = pos;
            return t;
        }

        Geometry geometry() const override {
            return {
                vertices.data(),
                static_cast<GLsizei>(vertices.size()),
                texture_coordinate.data(),
                3,
                indices.data(),
                static_cast<GLsizei>(indices.size())
            };
        }
    };

    class Quad : public BaseShape {
    public:
        std::array<glm::vec3, 4> vertices;
        std::array<unsigned int, 6> indices{0, 1, 3, 1, 2, 3};
        std::array<glm::vec2, 4> texture_coordinate {
            glm::vec2{1.f, 1.f},
            glm::vec2{1.f, 0.f},
            glm::vec2{0.f, 0.f},
            glm::vec2{0.f, 1.f}
        };

        static Quad make(std::array<glm::vec3, 4> verts, glm::vec3 pos = {0.f, 0.f, 0.f}){
            Quad q;
            q.vertices = verts;
            q.position = pos;
            return q;
        }

        Geometry geometry() const override {
            return {
                vertices.data(),
                static_cast<GLsizei>(vertices.size()),
                texture_coordinate.data(),
                4,
                indices.data(),
                static_cast<GLsizei>(indices.size())
            };
        }
    };
}

class Batch : public BaseShape {
public:
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::vec2> texture_coordinate;

    static Batch make(const std::vector<std::unique_ptr<BaseShape>>& shapes, glm::vec3 pos = {0.f, 0.f, 0.f}) {
        Batch b;
        b.position = pos;

        for (const auto& shape : shapes) {
            Geometry g = shape->geometry();

            glm::mat4 model{1.f};
            model = glm::translate(model, shape->position);

            model = glm::rotate(
                model,
                shape->orientation.x,
                glm::vec3(1, 0, 0)
            );

            model = glm::rotate(
                model,
                shape->orientation.y,
                glm::vec3(0, 1, 0)
            );

            model = glm::rotate(
                model,
                shape->orientation.z,
                glm::vec3(0, 0, 1)
            );

            model = glm::scale(model, shape->scale);

            const unsigned int vertex_offset = static_cast<unsigned int>(b.vertices.size());

            for (GLsizei i = 0; i < g.vertex_count; ++i) {
                glm::vec4 transformed = model * glm::vec4(g.vertices[i], 1.0f);

                b.vertices.push_back(glm::vec3(transformed));
            }

            for (GLsizei i = 0; i < g.texture_coordinate_count; ++i) {
                b.texture_coordinate.push_back(g.texture_coordinates[i]);
            }

            for (GLsizei i = 0; i < g.index_count; ++i) {
                b.indices.push_back(g.indices[i] + vertex_offset);
            }
        }
        return b;
    }

    Geometry geometry() const override {
        return {
            vertices.data(),
            static_cast<GLsizei>(vertices.size()),
            texture_coordinate.data(),
            static_cast<GLsizei>(texture_coordinate.size()),
            indices.data(),
            static_cast<GLsizei>(indices.size())
        };
    }
};

}

#endif // !GL_DRAW_STUFF_SHAPE_HPP

