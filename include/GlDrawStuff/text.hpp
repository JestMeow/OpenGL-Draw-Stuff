#ifndef TEXT_HPP
#define TEXT_HPP


#include <vector>
#include <array>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/common.hpp>

#include <GlDrawStuff/shape.hpp>
#include <GlDrawStuff/texture.hpp>

namespace GlDrawStuff {

namespace {
glm::vec2 index_position(float index) {
        float x = glm::mod(index, 16.f);
        float y = 15.f - glm::trunc(index / 16.f);

        return glm::vec2{x, y};
    }
};

class Text {
private:
    std::string m_text_content;
    Texture m_texture;

    float m_width = 1;
    float m_height = 1;

    Text(std::string text_content, Texture& texture, float width, float height) : m_text_content(std::move(text_content)), m_texture(texture), m_width(width), m_height(height) {}

    

public:
    std::vector<glm::vec3> position;

    static Text make(std::string text_content, Texture& texture, float width, float height) {
        return Text(std::move(text_content), texture, width, height);
    }

    Batch create() {
        if (m_text_content.empty()) return {};

        std::vector<std::unique_ptr<BaseShape>> characters;

        constexpr float atlas_dim = 1.f / 16;

        unsigned int cursor = 0;
        unsigned int line = 0;

        for (auto ch : m_text_content) {
            if (ch == '\n') {
                cursor = 0;
                line += 1;
                continue;
            }

            auto character = std::make_unique<Shape::Quad>(Shape::Quad::make(std::array<glm::vec3, 4>{
                glm::vec3(1.f * m_width , 1.f * m_height, 0.f),
                glm::vec3(1.f * m_width , 0.f           , 0.f),
                glm::vec3(0.f           , 0.f           , 0.f),
                glm::vec3(0.f           , 1.f * m_height, 0.f)
            }));

            int ch_ascii = static_cast<float>(ch);

            glm::vec2 index_pos = index_position(ch_ascii) * atlas_dim;

            character->texture_coordinate = std::array<glm::vec2, 4>{
                glm::vec2{index_pos.x + atlas_dim   , index_pos.y + atlas_dim   },
                glm::vec2{index_pos.x + atlas_dim   , index_pos.y               },
                index_pos                                                       ,
                glm::vec2{index_pos.x               , index_pos.y + atlas_dim   }
            };

            character->position.x += m_width * cursor;
            character->position.y -= m_height * line;
            
            characters.push_back(std::move(character));

            cursor++;
        }

        auto text_batch = Batch::make(characters);
        text_batch.set_texture(m_texture);
        return text_batch;
    }

    std::string get_text() {
        return m_text_content;
    }
};


}


#endif // !TEXT_HPP

