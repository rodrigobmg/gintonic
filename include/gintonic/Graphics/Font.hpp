/**
 * @file Font.hpp
 * @brief Defines a font class and a fontstream class. You usually don't use
 * fonts directly, but rather use fontstreams.
 * @author Raoul Wols
 */

#pragma once

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>
#include <gintonic/Asset.hpp>
#include <gintonic/Foundation/FilePath.hpp>
#include <gintonic/Graphics/OpenGL/BufferObject.hpp>
#include <gintonic/Graphics/OpenGL/TextureObject.hpp>
#include <gintonic/Graphics/OpenGL/VertexArrayObject.hpp>
#include <gintonic/Math/vec2f.hpp>
#include <iosfwd>

#include <gintonic/nsbegin.hpp>

/**
 * @brief      A font.
 */
class Font : public Asset
{

    GINTONIC_ASSET_BOILERPLATE(Font, "fonts", ".font");
    GINTONIC_ASSET_BOILERPLATE_TRIVIAL_CONSTRUCTORS(Font);

  public:
    /**
     * @brief      Gets thrown when the Font constructor failed to load the
     *             given file.
     */
    class InitException : public std::invalid_argument
    {
      public:
        ~InitException() noexcept override = default;

        /**
         * @brief      Get the erroneous filepath.
         *
         * @return     The erroneous filepath.
         */
        const FilePath& getFilePath() const noexcept;

      private:
        friend class Font;
        InitException(FilePath filepath);
        FilePath mFilePath;
    };

    /**
     * @brief      The character type. No widechar support.
     */
    typedef char char_type;

    /**
     * @brief      Construct a Font from the given filename with the given point
     *             size.
     *
     * @param[in]  filename       The filename where the TTF font resides.
     * @param[in]  pointsize      The point size in pixels.
     * @throws     InitException  when the supplied filename could not be
     *                            loaded.
     */
    std::shared_ptr<Font> fromNativeFontFile(const char* filename,
                                             const unsigned int pointsize);

    /**
     * @brief      Construct a Font from the given filename with the given point
     *             size.
     *
     * @param[in]  filename       The filename where the TTF font resides.
     * @param[in]  pointsize      The point size in pixels.
     * @throws     InitException  when the supplied filename could not be
     *                            loaded.
     */
    std::shared_ptr<Font> fromNativeFontFile(const std::string& filename,
                                             const unsigned int pointsize);

    /**
     * @brief      Construct a Font from the given filename with the given point
     *             size.
     *
     * @param[in]  filename       The filename where the TTF font resides.
     * @param[in]  pointsize      The point size in pixels.
     * @throws     InitException  when the supplied filename could not be
     *                            loaded.
     */
    std::shared_ptr<Font> fromNativeFontFile(FilePath filename,
                                             const unsigned int pointsize);

  public:
    /**
     * @brief      Returns the point size in pixels.
     *
     * @return     The point size in pixels.
     */
    inline unsigned int getPointSize() const noexcept { return mPointSize; }

    /**
     * @brief      Default destructor.
     */
    ~Font() noexcept override = default;

    /**
     * @brief      Draw this font. Some shader must be activated before calling
     *             this method.
     *
     * @param      text                 The text to render.
     * @param      length               The length of the text.
     * @param      position             The position, in `CLIP` space
     *                                  coordinates, of where to start rendering
     *                                  the text.
     * @param      scale                The scale of the text.
     * @param      maxHorizontalOffset  the maximal horizontal offset of the
     *                                  text.
     *
     * @return     The position of the last drawn character.
     */
    vec2f draw(const char* text, const std::size_t length,
               const vec2f& position, const vec2f& scale,
               const float maxHorizontalOffset = 10000.0f) const noexcept;

    /**
     * @brief      Draw this font. Some shader must be activated before calling
     *             this method.
     *
     * @param      text      The text to render.
     * @param      position  The position, in `CLIP` space coordinates, of where
     *                       to start rendering the text.
     * @param      scale     The scale of the text.
     */
    void draw(const std::string& text, const vec2f& position,
              const vec2f& scale) const noexcept;

    /**
     * @brief      Draw this font. Some shader must be activated before calling
     *             this method. The scale is chosen such that it corresponds to
     *             the point size of this font.
     *
     * @param      text      The text to render.
     * @param      length    The length of the text.
     * @param      position  The position, in `CLIP` space coordinates, of where
     *                       to start rendering the text.
     */
    void draw(const char* text, const std::size_t length,
              const vec2f& position) const noexcept;

    /**
     * @brief      Draw this font. Some shader must be activated before calling
     *             this method. The scale is chosen such that it corresponds to
     *             the point size of this font.
     *
     * @param      text      The text to render.
     * @param      position  The position, in `CLIP` space coordinates, of where
     *                       to start rendering the text.
     */
    void draw(const std::string& text, const vec2f& position) const noexcept;

  private:
    struct CharacterInfo
    {
        int16_t ax; // advance x
        int16_t ay; // advance y
        int16_t bw; // bitmap width
        int16_t bh; // bitmap height
        int16_t bl; // bitmap left
        int16_t bt; // bitmap top
        GLfloat tx; // x offset of glyph in texture coordinates
    } mChar[96];

    unsigned int mPointSize;

    GLsizei mAtlasWidth, mAtlasHeight;

    OpenGL::TextureObject mTextureObject;
    OpenGL::VertexArrayObject mVertexArrayObject;
    OpenGL::BufferObject mBufferObject;

    std::string mFontFile;
    void loadFromFileNow();

    template <class Archive>
    void save(Archive& archive, const unsigned /*version*/) const
    {
        using namespace boost::serialization;
        archive << BOOST_SERIALIZATION_BASE_OBJECT_NVP(Asset)
                << make_nvp("font_file", mFontFile)
                << make_nvp("point_size", mPointSize);
    }

    template <class Archive>
    void load(Archive& archive, const unsigned /*version*/)
    {
        using namespace boost::serialization;
        archive >> BOOST_SERIALIZATION_BASE_OBJECT_NVP(Asset) >>
            make_nvp("font_file", mFontFile) >>
            make_nvp("point_size", mPointSize);
        loadFromFileNow();
    }
};

//!@cond
namespace Detail
{
/**
 * @brief      Class for font stream.
 */
class FontStream
{
  public:
    typedef char char_type;
    typedef boost::iostreams::sink_tag category;

    /**
     * @brief      { function_description }
     *
     * @param[in]  f     { parameter_description }
     */
    FontStream(std::shared_ptr<Font> f);

    std::shared_ptr<Font> underlyingFont;

    vec2f scale;
    vec2f position;

    /**
     * @brief      { function_description }
     *
     * @param[in]  text    The text
     * @param[in]  length  The length
     *
     * @return     { description_of_the_return_value }
     */
    std::streamsize write(const char* text, const std::streamsize length) const
        noexcept;

    /**
     * @brief      { function_description }
     */
    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
};
} // namespace Detail
//!@endcond

/**
 * @brief      A font stream.
 */
typedef boost::iostreams::stream<Detail::FontStream> FontStream;

#include <gintonic/nsend.hpp>
