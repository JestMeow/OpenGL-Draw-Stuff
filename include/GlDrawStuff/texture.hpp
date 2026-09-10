#ifndef GL_DRAW_STUFF_TEXTURE_HPP
#define GL_DRAW_STUFF_TEXTURE_HPP


#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>

namespace GlDrawStuff {

class Texture {
public:
    unsigned int texture;
    
    Texture(const unsigned char* texture_bytes, int texture_len, GLint min_filter = GL_NEAREST_MIPMAP_NEAREST, GLint mag_filter = GL_NEAREST) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

        int width, height, nr_channels;

        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load_from_memory(
            texture_bytes,
            texture_len,
            &width,
            &height,
            &nr_channels,
            0
        );

        if (data) {
            GLenum format;

            if (nr_channels == 1)
                format = GL_RED;
            else if (nr_channels == 3)
                format = GL_RGB;
            else if (nr_channels == 4)
                format = GL_RGBA;
            else {
                std::cout << "Unsupported number of channels: "
                          << nr_channels << '\n';

                stbi_image_free(data);
                glBindTexture(GL_TEXTURE_2D, 0);
                return;
            }

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                format,
                width,
                height,
                0,
                format,
                GL_UNSIGNED_BYTE,
                data
            );

            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture: "
                      << stbi_failure_reason() << '\n';
        }

        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

}


#endif // !GL_DRAW_STUFF_TEXTURE_HPP
