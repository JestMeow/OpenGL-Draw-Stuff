#include <iostream>
#include <memory>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <GlDrawStuff/renderer.hpp>
#include <GlDrawStuff/shader.hpp>
#include <GlDrawStuff/shape.hpp>
#include <GlDrawStuff/texture.hpp>
#include <GlDrawStuff/text.hpp>

#include "container.h"
#include "flower_dandelion.h"
#include "default8.h"

#include "font_sbmff.h"


namespace WindowDimensions {
    int width = 600;
    int height = 600;

    int vp_width = width;
    int vp_height = height;

    int vp_x = 0;
    int vp_y = 0;
}

void move_rec(GLFWwindow *window, GlDrawStuff::BaseShape& shape) {
    const float speed = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
        shape.position.x += speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT)) {
        shape.position.x -= speed;
    }
    if (glfwGetKey(window, GLFW_KEY_UP)) {
        shape.position.y += speed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN)) {
        shape.position.y -= speed;
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    WindowDimensions::vp_width = width;
    WindowDimensions::vp_height = height;

    if (height > width) {
        WindowDimensions::vp_height = width;
    } else {
        WindowDimensions::vp_width = height;
    }

    WindowDimensions::vp_x = (width - WindowDimensions::vp_width) / 2;
    WindowDimensions::vp_y = (height - WindowDimensions::vp_height) / 2;
}


int main() {
    std::cout << "Yoooooo\n";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WindowDimensions::width, WindowDimensions::height, "OpenGL Draw Stuff Test", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }


    glViewport(0, 0, WindowDimensions::width, WindowDimensions::height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);




    GlDrawStuff::Shader default_shader;
    default_shader.use_default_shader();
    
    GlDrawStuff::Renderer renderer(1024 * 1024, 1024 * 1024, default_shader);



    auto triangle = GlDrawStuff::Shape::Triangle::make(std::array<glm::vec3, 3>{
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(1.f, 0.f, 0.f),
            glm::vec3(1.f, 1.f, 0.f)
        });
    
    GlDrawStuff::Mesh triangle_mesh = renderer.upload(triangle);



    GlDrawStuff::Texture texture(container_jpg, container_jpg_len, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    auto rectangle = GlDrawStuff::Shape::Quad::make(std::array<glm::vec3, 4>{
            glm::vec3(1.f, 1.f, 0.f),
            glm::vec3(1.f, 0.f, 0.f),
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.f, 1.f, 0.f)
        },
        glm::vec3(-2, 0, 0));
    
    //rectangle.color = glm::vec4(0.f, 1.f, 0.f, 1.f);
    rectangle.scale *= 2;


    rectangle.set_texture(texture);

    GlDrawStuff::Mesh rectangle_mesh = renderer.upload(rectangle);




    GlDrawStuff::Texture flower_texture(flower_dandelion_png, flower_dandelion_png_len);

    auto flower = GlDrawStuff::Shape::Quad::make(std::array<glm::vec3, 4>{
            glm::vec3(1.f, 1.f, 0.f),
            glm::vec3(1.f, 0.f, 0.f),
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.f, 1.f, 0.f)
        },
        glm::vec3(0, 0, 0));

    //rectangle.color = glm::vec4(0.f, 1.f, 0.f, 1.f);
    flower.scale *= 4;


    flower.set_texture(flower_texture);

    GlDrawStuff::Mesh flower_mesh = renderer.upload(flower);

    std::vector<std::unique_ptr<GlDrawStuff::BaseShape>> shapes;

    shapes.emplace_back(
            std::make_unique<GlDrawStuff::Shape::Quad>(flower)
            );
    shapes.emplace_back(
            std::make_unique<GlDrawStuff::Shape::Quad>(rectangle)
            );

    auto batch = GlDrawStuff::Batch::make(shapes);
    batch.set_texture(flower_texture);
    GlDrawStuff::Mesh batch_mesh = renderer.upload(batch);
    
    batch.scale *= 2;


    // FYI, sbmff stands for Simple Bitmap Font Format. I couldn't think of a better name, so just accept the long file extension... Or don't use the file extension at all.
    std::string sbmff_content(
        reinterpret_cast<char*>(font_sbmff), font_sbmff_len
    );

    std::cout << sbmff_content << '\n';

    auto ballz = GlDrawStuff::Text::create_font(sbmff_content);

    std::cout 
        << "Glyph Width: " << ballz.glyph_width 
        << "\nGlyph Heigh: " << ballz.glyph_height 
        << "\nAtlas Width: " << ballz.atlas_width
        << "\nDGlyph: " << ballz.default_width << ' ' << ballz.default_advance
        << '\n';

    for (const auto [key, glyph] : ballz.glyphs) {
        std::cout << "C: " << key << ' ' << glyph.ch << ' ' << glyph.width << ' ' << glyph.advance << '\n';
    }


    
    std::string text_content = "Hello,\nWorld!\nWOW!!!\n12345@#%^&\niii";

    GlDrawStuff::Texture font(default8_png, default8_png_len);
    auto text = GlDrawStuff::Text::make(text_content, font, ballz, 1.f, 1.f);
    std::cout << text.get_text() << '\n';

    auto text_batch = text.create();
    text_batch.position.x -= 5;
    text_batch.position.y += 2;
    text_batch.scale *= 1.5;
    GlDrawStuff::Mesh text_batch_mesh = renderer.upload(text_batch);




    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_SCISSOR_TEST);
        
        glViewport(WindowDimensions::vp_x, WindowDimensions::vp_y, WindowDimensions::vp_width, WindowDimensions::vp_height);
        glScissor(WindowDimensions::vp_x, WindowDimensions::vp_y, WindowDimensions::vp_width, WindowDimensions::vp_height);


        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDisable(GL_SCISSOR_TEST);


        move_rec(window, batch);

        renderer.begin();

        //renderer.draw(triangle, triangle_mesh);
        //renderer.draw(rectangle, rectangle_mesh);
        //renderer.draw(flower, flower_mesh);
        renderer.draw(batch, batch_mesh);
        renderer.draw(text_batch, text_batch_mesh);



        renderer.end();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}
