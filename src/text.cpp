#include <GlDrawStuff/text.hpp>

#include <sstream>

#include <GlDrawStuff/shape.hpp>

namespace GlDrawStuff {

class Texture;

namespace {
/**
 * @brief Converts a character index into a position in the font atlas.
 *
 * @param index Character index used to locate the glyph in the atlas.
 * @param atlas_width Number of glyph cells per row in the atlas.
 * @return Position of the glyph in atlas cell coordinates.
 */
glm::vec2 index_position(unsigned int index, unsigned int atlas_width) {
    unsigned int x = index % atlas_width;
    unsigned int y = atlas_width - 1 - glm::trunc(index / 16.f);

    return glm::vec2{x, y};
}

};


Text::Text(std::string text_content, Texture& texture, Font& font, float width, float height) :
    m_text_content(std::move(text_content)),
    m_texture(texture),
    m_font(font),
    m_width(width),
    m_height(height) {}

Text Text::make(std::string text_content, Texture& texture, Font& font, float width, float height) {
    return Text(std::move(text_content), texture, font, width, height);
}

Font Text::create_font(const std::string& contents) {
    Font f;

    std::istringstream ss(contents);
    std::string token;

    while (ss >> token) {
        int a, b;

        if (token == "GLYPH") {
            ss >> f.glyph_width >> f.glyph_height;
        }
        else if (token == "ATLAS") {
            ss >> f.atlas_width;
        }
        else if (token == "LINE_SPACE") {
            ss >> f.line_space;
        }
        else if (token == "DEFAULT") {
            ss >> f.default_width >> f.default_advance;
        }
        else if (token[0] == '.') {
            Glyph glyph;
            glyph.ch = token[1];

            ss >> glyph.width >> glyph.advance;
            f.glyphs[glyph.ch] = glyph;
        }
    }

    return f;
}

Batch Text::create() {
    if (m_text_content.empty()) return {};

    std::vector<std::unique_ptr<BaseShape>> characters;

    float atlas_cell = 1.f / m_font.atlas_width;
    float tex_width = m_font.glyph_width * m_font.default_width;

    float cursor = 0.f;
    float line = 0;

    for (auto ch : m_text_content) {
        if (ch == '\n') {
            cursor = 0;
            line += 1 + m_font.line_space / m_font.default_width;
            continue;
        }

        float glyph_width = m_font.default_width;
        float glyph_advance = m_font.default_advance;

        if (m_font.glyphs.find(ch) != m_font.glyphs.end()) {
            glyph_width = m_font.glyphs[ch].width;
            glyph_advance = m_font.glyphs[ch].advance;
        }

        auto character = std::make_unique<Shape::Quad>(Shape::Quad::make(std::array<glm::vec3, 4>{
            glm::vec3(1.f * m_width * glyph_width / m_font.default_width, 1.f * m_height, 0.f),
            glm::vec3(1.f * m_width * glyph_width / m_font.default_width, 0.f           , 0.f),
            glm::vec3(0.f                                               , 0.f           , 0.f),
            glm::vec3(0.f                                               , 1.f * m_height, 0.f)
        }));

        unsigned int ch_ascii = static_cast<unsigned int>(ch);

        glm::vec2 index_pos = index_position(ch_ascii, m_font.atlas_width) * atlas_cell;

        character->texture_coordinate = std::array<glm::vec2, 4>{
            glm::vec2{index_pos.x + atlas_cell * glyph_width / m_font.default_width , index_pos.y + atlas_cell  },
            glm::vec2{index_pos.x + atlas_cell * glyph_width / m_font.default_width , index_pos.y               },
            index_pos                                                                                           ,
            glm::vec2{index_pos.x                                                   , index_pos.y + atlas_cell  }
        };

        character->position.x += m_width * cursor;
        character->position.y -= m_height * line;
        
        characters.push_back(std::move(character));

        cursor += glyph_advance / m_font.default_width;
    }

    auto text_batch = Batch::make(characters);
    text_batch.set_texture(m_texture);
    return text_batch;
}

const std::string& Text::get_text() {
    return m_text_content;
}

};

