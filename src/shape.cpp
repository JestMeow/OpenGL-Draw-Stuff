#include <GlDrawStuff/shape.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GlDrawStuff {

class Shader;
class Texture;

void BaseShape::set_shader(Shader& s) & {
    m_shader = &s;
}

void BaseShape::set_texture(Texture& t) & {
    m_texture = &t;
}

void BaseShape::set_color(glm::vec4 c) {
    color = c;
}

Texture* BaseShape::get_texture() const {
    return m_texture;
}

namespace Shape {
Triangle Triangle::make(std::array<glm::vec3, 3> verts, glm::vec3 pos) {
    Triangle t;
    t.vertices = verts;
    t.position = pos;
    return t;
}

Geometry Triangle::geometry() const {
    return {
        vertices.data(),
        static_cast<GLsizei>(vertices.size()),
        texture_coordinate.data(),
        3,
        indices.data(),
        static_cast<GLsizei>(indices.size())
    };
}

Quad Quad::make(std::array<glm::vec3, 4> verts, glm::vec3 pos){
    Quad q;
    q.vertices = verts;
    q.position = pos;
    return q;
}

Geometry Quad::geometry() const {
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

Batch Batch::make(const std::vector<std::unique_ptr<BaseShape>>& shapes, glm::vec3 pos) {
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

Geometry Batch::geometry() const {
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

