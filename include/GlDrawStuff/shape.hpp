#ifndef GL_DRAW_STUFF_SHAPE_HPP
#define GL_DRAW_STUFF_SHAPE_HPP

#include <memory>

#include <array>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace GlDrawStuff {

class Shader;
class Texture;

/**
 * @brief Describes the CPU side geometry of a shape.
 *
 * Geometry provides non owning pointers to a shape's vertex positions,
 * texture coordinates, and indices. The referenced data must remain valid
 * for as long as the Geometry object is used.
 */
struct Geometry{
    const glm::vec3* vertices;
    GLsizei vertex_count;

    const glm::vec2* texture_coordinates;
    GLsizei texture_coordinate_count;

    const unsigned int* indices;
    GLsizei index_count;
};

/**
 * @brief Abstraction base class for renderable geometric shapes.
 *
 * BaseShape stores the common transformation, color, shader, and texture
 * state shared by all shapes. Derived classes provide their geometry by
 * implementing geometry().
 *
 * Shader and Texture references are non owning. The referenced objects
 * must outlive the BaseShape that uses them.
 */
class BaseShape {
private:
    // NON-OWNING. Must outlive this shape object.
    Shader* m_shader = nullptr;
    // NON-OWNING. Must outlive this shape object.
    Texture* m_texture = nullptr;

public:
    /**
     * @brief Shape color.
     *
     * Used by the renderer when drawing the shape.
     */
    glm::vec4 color{1.f};

    /**
     * @brief Position of the shape in world space.
     */
    glm::vec3 position{0.f};

    /**
     * @brief Rotation of the shape around the X, Y, and Z axes.
     *
     * The values are interpreted as angles in radians.
     */
    glm::vec3 orientation{0.f};

    /**
     * @brief Scale applied to the shape along each axes.
     */
    glm::vec3 scale{1.f};
    

    /**
     * @brief Virtual destructor for polymorphic shape objects.
     */
    virtual ~BaseShape() = default;

    /**
     * @brief Returns the geometry associated with the shape.
     *
     * @return Non owning description of the shapes's vertex data,
     * texture coordinates, and indices.
     */
    virtual Geometry geometry() const = 0;


    /**
     * @brief Assigns a shader to a the shape.
     *
     * The shader is stored as a non owning pointer and must remain valid
     * for the lifetime of the shape or until another shader is assigned.
     *
     * @param s Shader to associate with the shape.
     */
    void set_shader(Shader& s) &;
    /**
     * @brief Prevents assigning a shader to a temporary shape.
     */
    void set_shader(Shader& s) && = delete;

    /**
     * @brief Assigns a texture to the shape.
     *
     * The texture is stored as a non owning pointer and must remain valid
     * for the lifetime of the shape or until another texture is assigned.
     *
     * @param t Texture to associate with the shape.
     */
    void set_texture(Texture& t) &;
    /**
     * @brief Prevents assigning a texture to a temporary shape.
     */
    void set_texture(Texture& t) && = delete;

    /**
     * @brief Sets the color of the shape.
     *
     * @param c New RGBA color.
     */
    void set_color(glm::vec4 c);

    /**
     * @brief Gets the texture assigned to the shape.
     *
     * @return Non owning pointer to the assigned texture, or nullptr
     * if no texture has been assigned.
     */
    Texture* get_texture() const;
};

/**
 * @brief Provides predefined geometric shapes.
 */
namespace Shape {
    /**
     * @brief Represents a triangle.
     *
     * The triangle contains three vertices, three texture coordinates,
     * and three indices.
     */
    class Triangle : public BaseShape {
    public:
        std::array<glm::vec3, 3> vertices;
        std::array<glm::vec2, 3> texture_coordinate {
            glm::vec2{1.f, 1.f},
            glm::vec2{1.f, 0.f},
            glm::vec2{0.f, 0.f}
        };
        std::array<unsigned int, 3> indices{0, 1, 2};

        /**
         * @brief Creates a triangle from three vertex positions.
         *
         * @param verts Vertex positions of the triangle.
         * @param pos Initial position of the triangle.
         * @return A newly constructed triangle.
         */
        static Triangle make(std::array<glm::vec3, 3> verts, glm::vec3 pos = {0.f, 0.f, 0.f});

        /**
         * @brief Returns the triangle's geometry.
         *
         * @return Geometry referencing the triangle's vertex,
         * texture coordinates, and index data.
         */
        Geometry geometry() const override;
    };

    /**
     * @brief Represents a quadrilateral composed of two triangles.
     *
     * The quad contains four vertices and six indices, forming two
     * triangles for rendering with GL_TRIANGLES.
     */
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

        /**
         * @brief Creates a quad from four vertex positions.
         *
         * @param verts Vertex positions of the quad.
         * @param pos Initial position of the quad.
         * @return A newly constructed quad.
         */
        static Quad make(std::array<glm::vec3, 4> verts, glm::vec3 pos = {0.f, 0.f, 0.f});

        /**
         * @brief Returns the quad's geometry.
         *
         * @return Geometry referencing the quad's vertex,
         * texture coordinates, and index data.
         */
        Geometry geometry() const override;
    };
}

/**
 * @brief Combines multiple shapes into a single shape.
 *
 * Batch stores the geometry of multiple BaseShape objects in singular
 * vertex, texture coordinate, and index array.
 *
 * Each source shape's position, rotation, and scale are applied to its
 * vertices when the batch is created. The resulting vertices are stored
 * directly in the batch's local geometry.
 *
 * This can be used to combine multiple shapes into a single draw call.
 */
class Batch : public BaseShape {
public:
    /**
     * @brief Combined vertex positions of all shapes in the batch.
     */
    std::vector<glm::vec3> vertices;
    /**
     * @brief Combined indices of all shapes in the batch.
     *
     * Indices are adjusted to account for each shape's position
     * within the combined vertex array.
     */
    std::vector<unsigned int> indices;
    /**
     * @brief Combined texture coordinates of all shapes in the batch.
     */
    std::vector<glm::vec2> texture_coordinate;

    /**
     * @brief Creates a batch from multiple shapes.
     *
     * The geometry of each shape is copied into the batch. Each shape's
     * position, orientation, and scale are applied to its vertices before
     * they are stored in the batch
     *
     * Indices are automatically adjusted to reference the corresponding
     * vertices in the combined vertex array.
     *
     * @param shapes Shapes whos geometry should be combined.
     * @param pos Initial position of the resulting batch.
     * @return A newly constructed Batch containing the combined geometry.
     */
    static Batch make(const std::vector<std::unique_ptr<BaseShape>>& shapes, glm::vec3 pos = {0.f, 0.f, 0.f});

    /**
     * @brief Returns the combined geometry of the batch.
     *
     * @return Geometry referencing to the batch's vertex,
     * texture coordinate, and index arrays.
     */
    Geometry geometry() const override;
};

}

#endif // !GL_DRAW_STUFF_SHAPE_HPP

