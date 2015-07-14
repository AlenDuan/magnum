#ifndef Magnum_ColorFormat_h
#define Magnum_ColorFormat_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Enum @ref Magnum::ColorFormat, @ref Magnum::ColorType
 */

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief Format of image data

Note that some formats can be used only for framebuffer reading (using
@ref AbstractFramebuffer::read()) and some only for texture data (using
@ref Texture::setSubImage() and others), the limitations are mentioned in
documentation of each particular value.

In most cases you may want to use @ref ColorFormat::Red (for grayscale images),
@ref ColorFormat::RGB or @ref ColorFormat::RGBA along with
@ref ColorType::UnsignedByte, the matching texture format is then
@ref TextureFormat::R8, @ref TextureFormat::RGB8 or @ref TextureFormat::RGBA8.
See documentation of these values for possible limitations when using OpenGL ES
2.0 or WebGL.

@see @ref Image, @ref ImageReference, @ref BufferImage, @ref Trade::ImageData
*/
enum class ColorFormat: GLenum {
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Floating-point red channel.
     * @requires_gles30 For texture data only, extension
     *      @es_extension{EXT,texture_rg} in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading, extension
     *      @es_extension{EXT,texture_rg}
     * @requires_webgl20 For texture data only. Not available in WebGL 1.0, see
     *      @ref ColorFormat::Luminance for an alternative.
     */
    #ifndef MAGNUM_TARGET_GLES2
    Red = GL_RED,
    #else
    Red = GL_RED_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Floating-point green channel.
     * @requires_gl Only @ref ColorFormat::Red is available in OpenGL ES or
     *      WebGL.
     */
    Green = GL_GREEN,

    /**
     * Floating-point blue channel.
     * @requires_gl Only @ref ColorFormat::Red is available in OpenGL ES or
     *      WebGL.
     */
    Blue = GL_BLUE,
    #endif

    #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * Floating-point luminance channel. The value is used for all RGB
     * channels.
     * @requires_gles20 Not available in ES 3.0, WebGL 2.0 or desktop OpenGL.
     *      Use @ref ColorFormat::Red instead.
     * @deprecated_gl Included for compatibility reasons only, use
     *      @ref ColorFormat::Red instead.
     */
    Luminance = GL_LUMINANCE,
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Floating-point red and green channel.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 For texture data only, extension
     *      @es_extension{EXT,texture_rg} in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading, extension
     *      @es_extension{EXT,texture_rg}
     * @requires_webgl20 For texture data only. Not available in WebGL 1.0, see
     *      @ref ColorFormat::LuminanceAlpha for and alternative.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RG = GL_RG,
    #else
    RG = GL_RG_EXT,
    #endif
    #endif

    #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * Floating-point luminance and alpha channel. First value is used for all
     * RGB channels, second value is used for alpha channel.
     * @requires_gles20 Not available in ES 3.0, WebGL 2.0 or desktop OpenGL.
     *      Use @ref ColorFormat::RG instead.
     * @deprecated_gl Included for compatibility reasons only, use
     *      @ref ColorFormat::RG instead.
     */
    LuminanceAlpha = GL_LUMINANCE_ALPHA,
    #endif

    /**
     * Floating-point RGB.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or WebGL.
     */
    RGB = GL_RGB,

    /** Floating-point RGBA. */
    RGBA = GL_RGBA,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Floating-point BGR.
     * @requires_gl Only RGB component ordering is available in OpenGL ES and
     *      WebGL.
     */
    BGR = GL_BGR,
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Floating-point BGRA.
     * @requires_es_extension Extension @es_extension{EXT,read_format_bgra}
     *      for framebuffer reading, extension @es_extension{APPLE,texture_format_BGRA8888}
     *      or @es_extension{EXT,texture_format_BGRA8888} for texture data.
     * @requires_gles Only RGBA component ordering is available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    BGRA = GL_BGRA,
    #else
    BGRA = GL_BGRA_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Integer red channel.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only floating-point image data are available in OpenGL
     *      ES 2.0.
     * @requires_webgl20 Only floating-point image data are available in WebGL
     *      1.0.
     */
    RedInteger = GL_RED_INTEGER,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Integer green channel.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gl Only @ref ColorFormat::RedInteger is available in OpenGL ES
     *      3.0 and WebGL 2.0, only floating-point image data are available in
     *      OpenGL ES 2.0 and WebGL 1.0.
     */
    GreenInteger = GL_GREEN_INTEGER,

    /**
     * Integer blue channel.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gl Only @ref ColorFormat::RedInteger is available in OpenGL ES
     *      3.0 and WebGL 2.0, only floating-point image data are available in
     *      OpenGL ES 2.0 and WebGL 1.0.
     */
    BlueInteger = GL_BLUE_INTEGER,
    #endif

    /**
     * Integer red and green channel.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_gles30 For texture data only, only floating-point image data
     *      are available in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, only floating-point image data
     *      are available in WebGL 1.0.
     */
    RGInteger = GL_RG_INTEGER,

    /**
     * Integer RGB.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_gles30 For texture data only, only floating-point image data
     *      are available in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, only floating-point image data
     *      are available in WebGL 1.0.
     */
    RGBInteger = GL_RGB_INTEGER,

    /**
     * Integer RGBA.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only floating-point image data are available in OpenGL
     *      ES 2.0.
     * @requires_webgl20 Only floating-point image data are available in WebGL
     *      1.0.
     */
    RGBAInteger = GL_RGBA_INTEGER,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Integer BGR.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gl Only @ref ColorFormat::RGBInteger is available in OpenGL ES
     *      3.0 and WebGL 2.0, only floating-point image data are available in
     *      OpenGL ES 2.0 and WebGL 1.0.
     */
    BGRInteger = GL_BGR_INTEGER,

    /**
     * Integer BGRA.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gl Only @ref ColorFormat::RGBAInteger is available in OpenGL
     *      ES 3.0 and WebGL 2.0, only floating-point image data are available
     *      in OpenGL ES 2.0 and WebGL 1.0.
     */
    BGRAInteger = GL_BGRA_INTEGER,
    #endif
    #endif

    /**
     * Depth component.
     * @requires_gles30 For texture data only, extension @es_extension{OES,depth_texture}
     *      or @es_extension{ANGLE,depth_texture} in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading only, extension
     *      @es_extension2{NV,read_depth,GL_NV_read_depth_stencil}.
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{WEBGL,depth_texture} in WebGL 1.0.
     */
    DepthComponent = GL_DEPTH_COMPONENT,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Stencil index.
     * @requires_gl44 Extension @extension{ARB,texture_stencil8} for texture
     *      data, otherwise for framebuffer reading only.
     * @requires_es_extension Extension @es_extension2{NV,read_stencil,GL_NV_read_depth_stencil},
     *      for framebuffer reading only.
     * @requires_es_extension Extension @es_extension{ANDROID,extension_pack_es31a}/
     *      @es_extension{OES,texture_stencil8}, for texture data only.
     * @requires_gles Stencil index is not available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    StencilIndex = GL_STENCIL_INDEX,
    #else
    StencilIndex = GL_STENCIL_INDEX_OES,
    #endif
    #endif

    /**
     * Depth and stencil.
     * @requires_gl30 Extension @extension{ARB,framebuffer_object}
     * @requires_gles30 For texture data only, extension @es_extension{OES,packed_depth_stencil}
     *      in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading only, extension
     *      @es_extension2{NV,read_depth_stencil,GL_NV_read_depth_stencil}
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{WEBGL,depth_texture} in WebGL 1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    DepthStencil = GL_DEPTH_STENCIL
    #else
    /* Using OES version even though WebGL 1.0 *has* DEPTH_STENCIL constant,
       because there are no such headers for it */
    DepthStencil = GL_DEPTH_STENCIL_OES
    #endif
};

/**
@brief Type of image data

Note that some formats can be used only for framebuffer reading (using
@ref AbstractFramebuffer::read()) and some only for texture data (using
@ref Texture::setSubImage() and others), the limitations are mentioned in
documentation of each particular value.

In most cases you may want to use @ref ColorType::UnsignedByte along with
@ref ColorFormat::Red (for grayscale images), @ref ColorFormat::RGB or
@ref ColorFormat::RGBA, the matching texture format is then
@ref TextureFormat::R8, @ref TextureFormat::RGB8 or @ref TextureFormat::RGBA8.
See documentation of these values for possible limitations when using OpenGL ES
2.0 or WebGL.

@see @ref Image, @ref ImageReference, @ref BufferImage, @ref Trade::ImageData
*/
enum class ColorType: GLenum {
    /** Each component unsigned byte. */
    UnsignedByte = GL_UNSIGNED_BYTE,

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Each component signed byte.
     * @requires_gles30 For texture data only, only @ref ColorType::UnsignedByte
     *      is available in OpenGL ES 2.0.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_webgl20 For texture data only, only @ref ColorType::UnsignedByte
     *      is available in WebGL 1.0.
     */
    Byte = GL_BYTE,
    #endif

    /**
     * Each component unsigned short.
     * @requires_gles30 For texture data only, extension @es_extension{OES,depth_texture}
     *      or @es_extension{ANGLE,depth_texture} in OpenGL ES 2.0.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{WEBGL,depth_texture} in WebGL 1.0.
     */
    UnsignedShort = GL_UNSIGNED_SHORT,

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Each component signed short.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_gles30 For texture data only, only @ref ColorType::UnsignedShort
     *      is available in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, only @ref ColorType::UnsignedShort
     *      is available in WebGL 1.0.
     */
    Short = GL_SHORT,
    #endif

    /**
     * Each component unsigned int.
     * @requires_gles30 For texture data only, extension @es_extension{OES,depth_texture}
     *      or @es_extension{ANGLE,depth_texture} in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{WEBGL,depth_texture} in WebGL 1.0.
     */
    UnsignedInt = GL_UNSIGNED_INT,

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Each component signed int.
     * @requires_gles30 Only @ref ColorType::UnsignedInt is available in OpenGL
     *      ES 2.0.
     * @requires_webgl20 Only @ref ColorType::UnsignedInt is available in WebGL
     *      1.0.
     */
    Int = GL_INT,
    #endif

    /**
     * Each component half float.
     * @requires_gl30 Extension @extension{ARB,half_float_pixel}
     * @requires_gles30 For texture data only, extension
     *      @es_extension2{OES,texture_half_float,OES_texture_float} in OpenGL
     *      ES 2.0.
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{OES,texture_half_float} in WebGL 1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    HalfFloat = GL_HALF_FLOAT,
    #else
    HalfFloat = GL_HALF_FLOAT_OES,
    #endif

    /**
     * Each component float.
     * @requires_gles30 For texture data only, extension @es_extension{OES,texture_float}
     *      in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, extension @webgl_extension{OES,texture_float}
     *      in WebGL 1.0.
     */
    Float = GL_FLOAT,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGB, unsigned byte, red and green component 3bit, blue component 2bit.
     * @requires_gl Packed 12bit types are not available in OpenGL ES or WebGL.
     */
    UnsignedByte332 = GL_UNSIGNED_BYTE_3_3_2,

    /**
     * BGR, unsigned byte, red and green component 3bit, blue component 2bit.
     * @requires_gl Packed 12bit types are not available in OpenGL ES or WebGL.
     */
    UnsignedByte233Rev = GL_UNSIGNED_BYTE_2_3_3_REV,
    #endif

    /**
     * RGB, unsigned byte, red and blue component 5bit, green 6bit.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or WebGL.
     */
    UnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * BGR, unsigned short, red and blue 5bit, green 6bit.
     * @requires_gl Only @ref ColorType::UnsignedShort565 is available in
     *      OpenGL ES or WebGL.
     */
    UnsignedShort565Rev = GL_UNSIGNED_SHORT_5_6_5_REV,
    #endif

    /**
     * RGBA, unsigned short, each component 4bit.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or WebGL.
     */
    UnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * ABGR, unsigned short, each component 4bit.
     * @requires_es_extension For framebuffer reading only, extension
     *      @es_extension{EXT,read_format_bgra}
     * @requires_gles Only RGBA component ordering is available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    UnsignedShort4444Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
    #else
    UnsignedShort4444Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT,
    #endif
    #endif

    /**
     * RGBA, unsigned short, each RGB component 5bit, alpha component 1bit.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or WebGL.
     */
    UnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * ABGR, unsigned short, each RGB component 5bit, alpha component 1bit.
     * @requires_es_extension For framebuffer reading only, extension
     *      @es_extension{EXT,read_format_bgra}
     * @requires_gles Not available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    UnsignedShort1555Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
    #else
    UnsignedShort1555Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGBA, unsigned int, each component 8bit.
     * @requires_gl Use @ref ColorType::UnsignedByte in OpenGL ES and WebGL
     *      instead.
     */
    UnsignedInt8888 = GL_UNSIGNED_INT_8_8_8_8,

    /**
     * ABGR, unsigned int, each component 8bit.
     * @requires_gl Only RGBA component ordering is available in OpenGL ES and
     *      WebGL, see @ref ColorType::UnsignedInt8888 for more information.
     */
    UnsignedInt8888Rev = GL_UNSIGNED_INT_8_8_8_8_REV,

    /**
     * RGBA, unsigned int, each RGB component 10bit, alpha component 2bit.
     * @requires_gl Only @ref ColorType::UnsignedInt2101010Rev is available in
     *      OpenGL ES and WebGL.
     */
    UnsignedInt1010102 = GL_UNSIGNED_INT_10_10_10_2,
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * ABGR, unsigned int, each RGB component 10bit, alpha component 2bit.
     * @requires_gles30 Can't be used for framebuffer reading in OpenGL ES 2.0.
     * @requires_gles30 For texture data only, extension
     *      @es_extension{EXT,texture_type_2_10_10_10_REV} in OpenGL ES 2.0.
     *      Not available in WebGL 1.0.
     * @requires_webgl20 Only RGBA component ordering is available in WebGL
     *      1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,
    #else
    UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * BGR, unsigned int, red and green 11bit float, blue 10bit float.
     * @requires_gl30 Extension @extension{EXT,packed_float}
     * @requires_gles30 Floating-point types are not available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Floating-point types are not available in WebGL 1.0.
     */
    UnsignedInt10F11F11FRev = GL_UNSIGNED_INT_10F_11F_11F_REV,

    /**
     * BGR, unsigned int, each component 9bit + 5bit exponent.
     * @requires_gl30 Extension @extension{EXT,texture_shared_exponent}
     * @requires_gles30 Only 8bit and 16bit types are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only 8bit and 16bit types are available in WebGL 1.0.
     */
    UnsignedInt5999Rev = GL_UNSIGNED_INT_5_9_9_9_REV,
    #endif

    /**
     * Unsigned int, depth component 24bit, stencil index 8bit.
     * @requires_gl30 Extension @extension{ARB,framebuffer_object}
     * @requires_gles30 For texture data only, extension
     *      @es_extension{OES,packed_depth_stencil} in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{WEBGL,depth_texture} in WebGL 1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    UnsignedInt248 = GL_UNSIGNED_INT_24_8,
    #else
    UnsignedInt248 = GL_UNSIGNED_INT_24_8_OES,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Float + unsigned int, depth component 32bit float, 24bit gap, stencil
     * index 8bit.
     * @requires_gl30 Extension @extension{ARB,depth_buffer_float}
     * @requires_gles30 For texture data only, only @ref ColorType::UnsignedInt248
     *      is available in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, only @ref ColorType::UnsignedInt248
     *      is available in WebGL 1.0.
     */
    Float32UnsignedInt248Rev = GL_FLOAT_32_UNSIGNED_INT_24_8_REV
    #endif
};

/** @debugoperatorenum{Magnum::ColorFormat} */
Debug MAGNUM_EXPORT operator<<(Debug debug, ColorFormat value);

/** @debugoperatorenum{Magnum::ColorType} */
Debug MAGNUM_EXPORT operator<<(Debug debug, ColorType value);

}

#endif
