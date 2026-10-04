#ifndef TEXT_HPP
#define TEXT_HPP

#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <cctype>

#include <glm/glm.hpp>
#include <glm/common.hpp>

#include <GlDrawStuff/shape.hpp>

namespace GlDrawStuff {

class Texture;

namespace {
/**
 * @brief Describes the metrics of a single character glyph.
 *
 * Glyphs stores the character represented by an entry in a font and
 * metrics used when generating its geometry and positioning subsequent
 * characters.
 */
struct Glyph {
    char ch;

    unsigned int width;
    unsigned int advance;
};

/**
 * @brief Describes a font and its glyph metrics.
 *
 * Font contains the dimensions of the font's glyph atlas together with
 * per character width and advance information.
 */
struct Font {
    float glyph_width, glyph_height;
    float atlas_width;

    float default_width, default_advance;
    float line_space;

    std::unordered_map<char, Glyph> glyphs;
};

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

/**
 * @brief Represents text rendered using texture based font atlas.
 *
 * Text converts a string into a collection of textured quads, with each
 * character represented by a glyph from the supplied font atlas.
 *
 * The generated geometry can be obtained using create(), which returns
 * a Batch containing all character quads.
 *
 * Text does not own the Texture or Font objects passed to it. Both objects
 * must outlive the Text instance.
 */
class Text {
private:
    std::string m_text_content;
    Texture& m_texture;
    Font& m_font;

    float m_width = 1;
    float m_height = 1;

    /**
     * @brief Constructs a Text object.
     *
     * @param text_content Text string to render.
     * @param texture Texture containing the font atlas.
     * @param font Font metrics describing the atlas.
     * @param width Horizontal scale applied to generated glyphs.
     * @param height Vertical scale applied to generated glyphs.
     */
    Text(std::string text_content, Texture& texture, Font& font, float width, float height) :
        m_text_content(std::move(text_content)),
        m_texture(texture),
        m_font(font),
        m_width(width),
        m_height(height) {}

public:
    /**
     * @brief Positions associated with the generated text.
     *
     * @note This member is currently not used when generating
     * geometry in create().
     */
    std::vector<glm::vec3> position;

    /**
     * @brief Creates a Text object.
     * 
     * @param text_content Text string to render.
     * @param texture Texture containing the font atlas.
     * @param font Font metrics describing the atlas.
     * @param width Horizontal scale applied to the generated glyphs.
     * @param height Vertical scale applied to the generated glyphs.
     */
    static Text make(std::string text_content, Texture& texture, Font& font, float width, float height) {
        return Text(std::move(text_content), texture, font, width, height);
    }

    /**
     * @brief Creates a Font from a textual description we can call
     * Simple Bitmap Font Format (SBMFF), but you can call it
     * whatever you want.
     *
     * The description is parsed as whitespace separated tokens. The
     * following shows the supported entries:
     * - GLYPH width height
     * - ATLAS width
     * - LINE_SPACE spacing
     * - DEFAULT width advance
     * - .character width advance
     * 
     * Character entries beginning with '.' define metrics for an individual
     * character.
     *
     * @param contents Textual font description to parse.
     * @return A Font containing the parsed font and glyph metrics.
     */
    static Font create_font(const std::string& contents) {
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

    /**
     * @brief Generates renderable geometry for the text.
     *
     * Each character in the text is converted into a textured quad using
     * the supplied font metrics and texture atlas. Newline characters
     * start a new line.
     *
     * Characters with explicit glyph metrics use their configured width
     * and advance values. Characters without explicit metrics use the
     * font's default width and advance.
     *
     * The returned Batch contains all generated character geometry and
     * references the texture supplied when the Text object was created.
     *
     * @return A Batch containing the generated text geometry.
     *
     * @note Returns an empty Batch when the text content is empty.
     */
    Batch create() {
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

    /**
     * @brief Returns the text content.
     *
     * @return Const reference to the string represented by this Text object.
     */
    const std::string& get_text() {
        return m_text_content;
    }
};

}


#endif // !TEXT_HPP

