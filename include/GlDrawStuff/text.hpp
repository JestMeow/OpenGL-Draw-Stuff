#ifndef TEXT_HPP
#define TEXT_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include <GlDrawStuff/shape.hpp>

namespace GlDrawStuff {

class Texture;

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
    Text(std::string text_content, Texture& texture, Font& font, float width, float height);

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
    static Text make(std::string text_content, Texture& texture, Font& font, float width, float height);

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
    static Font create_font(const std::string& contents);

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
    Batch create();

    /**
     * @brief Returns the text content.
     *
     * @return Const reference to the string represented by this Text object.
     */
    const std::string& get_text();
};

}


#endif // !TEXT_HPP

