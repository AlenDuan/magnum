#ifndef Magnum_TextureArray_h
#define Magnum_TextureArray_h
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

#ifndef MAGNUM_TARGET_GLES2
/** @file
 * @brief Class @ref Magnum::TextureArray, typedef @ref Magnum::Texture1DArray, @ref Magnum::Texture2DArray
 */
#endif

#include "Magnum/AbstractTexture.h"
#include "Magnum/Array.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"

#ifndef MAGNUM_TARGET_GLES2
namespace Magnum {

namespace Implementation {
    template<UnsignedInt> constexpr GLenum textureArrayTarget();
    /* GCC 4.5 doesn't have constexpr, so it needs inline */
    #ifndef MAGNUM_TARGET_GLES
    template<> constexpr inline GLenum textureArrayTarget<1>() { return GL_TEXTURE_1D_ARRAY; }
    #endif
    template<> constexpr inline GLenum textureArrayTarget<2>() { return GL_TEXTURE_2D_ARRAY; }
}

/**
@brief Texture array

Template class for one- and two-dimensional texture arrays. See also
@ref AbstractTexture documentation for more information.

## Usage

See @ref Texture documentation for introduction.

Common usage is to fully configure all texture parameters and then set the
data. Example configuration:
@code
Texture2DArray texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    .setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
    .setWrapping(Sampler::Wrapping::ClampToEdge)
    .setMaxAnisotropy(Sampler::maxMaxAnisotropy());;
@endcode

It is often more convenient to first allocate the memory for all layers by
calling @ref setStorage() and then specify each layer separately using
@ref setSubImage():
@code
texture.setStorage(levels, TextureFormat::RGBA8, {64, 64, 16});

for(std::size_t i = 0; i != 16; ++i) {
    Image3D image(ColorFormat::RGBA, ColorType::UnsignedByte, {64, 64, 1}, ...);
    texture.setSubImage(0, Vector3i::zAxis(i), image);
}
@endcode

In shader, the texture is used via `sampler1DArray`/`sampler2DArray`,
`sampler1DArrayShadow`/`sampler1DArrayShadow`, `isampler1DArray`/`isampler2DArray`
or `usampler1DArray`/`usampler2DArray`. See @ref AbstractShaderProgram
documentation for more information about usage in shaders.

@see @ref Texture1DArray, @ref Texture2DArray, @ref Texture,
    @ref CubeMapTexture, @ref CubeMapTextureArray, @ref RectangleTexture,
    @ref BufferTexture, @ref MultisampleTexture
@requires_gl30 Extension @extension{EXT,texture_array}
@requires_gles30 Array textures are not available in OpenGL ES 2.0.
@requires_webgl20 Array textures are not available in WebGL 1.0.
@requires_gl 1D array textures are not available in OpenGL ES or WebGL, only
    2D ones.
@todo Fix this when @es_extension{NV,texture_array} is in ES2 extension headers
 */
template<UnsignedInt dimensions> class TextureArray: public AbstractTexture {
    public:
        static const UnsignedInt Dimensions = dimensions; /**< @brief Texture dimension count */

        /**
         * @brief Max supported texture array size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{EXT,texture_array} (part of
         * OpenGL 3.0) is not available, returns zero vector.
         * @see @fn_gl{Get} with @def_gl{MAX_TEXTURE_SIZE} and
         *      @def_gl{MAX_ARRAY_TEXTURE_LAYERS}
         */
        static typename DimensionTraits<dimensions+1, Int>::VectorType maxSize();

        /**
         * @brief Wrap existing OpenGL texture array object
         * @param id            OpenGL texture array ID
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL texture object
         * with target @def_gl{TEXTURE_1D_ARRAY} or @def_gl{TEXTURE_2D_ARRAY}
         * based on dimension count. Unlike texture created using constructor,
         * the OpenGL object is by default not deleted on destruction, use
         * @p flags for different behavior.
         * @see @ref release()
         */
        /* GCC 4.6 needs explicit type */
        static TextureArray<dimensions> wrap(GLuint id, ObjectFlags flags = ObjectFlags{}) {
            return TextureArray<dimensions>{id, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref wrap(), @fn_gl{CreateTextures} with
         *      @def_gl{TEXTURE_1D_ARRAY} or @def_gl{TEXTURE_2D_ARRAY},
         *      eventually @fn_gl{GenTextures}
         */
        explicit TextureArray(): AbstractTexture(Implementation::textureArrayTarget<dimensions>()) {}

        /**
         * @copybrief Texture::setBaseLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBaseLevel() for more information.
         */
        TextureArray<dimensions>& setBaseLevel(Int level) {
            AbstractTexture::setBaseLevel(level);
            return *this;
        }

        /**
         * @copybrief Texture::setMaxLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLevel() for more information.
         */
        TextureArray<dimensions>& setMaxLevel(Int level) {
            AbstractTexture::setMaxLevel(level);
            return *this;
        }

        /**
         * @copybrief Texture::setMinificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinificationFilter() for more information.
         */
        TextureArray<dimensions>& setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /**
         * @copybrief Texture::setMagnificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMagnificationFilter() for more information.
         */
        TextureArray<dimensions>& setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        /**
         * @copybrief Texture::setMinLod()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinLod() for more information.
         */
        TextureArray<dimensions>& setMinLod(Float lod) {
            AbstractTexture::setMinLod(lod);
            return *this;
        }

        /**
         * @copybrief Texture::setMaxLod()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLod() for more information.
         */
        TextureArray<dimensions>& setMaxLod(Float lod) {
            AbstractTexture::setMaxLod(lod);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::setLodBias()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setLodBias() for more information.
         * @requires_gl Texture LOD bias can be specified only directly in
         *      fragment shader in OpenGL ES and WebGL.
         */
        TextureArray<dimensions>& setLodBias(Float bias) {
            AbstractTexture::setLodBias(bias);
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setWrapping()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setWrapping() for more information.
         */
        TextureArray<dimensions>& setWrapping(const Array<dimensions, Sampler::Wrapping>& wrapping) {
            DataHelper<dimensions>::setWrapping(*this, wrapping);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @copybrief Texture::setBorderColor(const Color4&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Color4&) for more
         * information.
         * @requires_es_extension Extension @es_extension{NV,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        TextureArray<dimensions>& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::setBorderColor(const Vector4ui&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Vector4ui&) for more
         * information.
         * @requires_gl30 Extension @extension{EXT,texture_integer}
         * @requires_gl Border clamp is available only for float textures in
         *      OpenGL ES. Border clamp is not available in WebGL.
         */
        TextureArray<dimensions>& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @overload
         * @requires_gl30 Extension @extension{EXT,texture_integer}
         * @requires_gl Border clamp is available only for float textures in
         *      OpenGL ES. Border clamp is not available in WebGL.
         */
        TextureArray<dimensions>& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setMaxAnisotropy()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxAnisotropy() for more information.
         */
        TextureArray<dimensions>& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @copybrief Texture::setSRGBDecode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSRGBDecode() for more information.
         * @requires_extension Extension @extension{EXT,texture_sRGB_decode}
         * @requires_es_extension Extension @es_extension2{EXT,texture_sRGB_decode,texture_sRGB_decode}
         * @requires_gles SRGB decode is not available in WebGL.
         */
        TextureArray<dimensions>& setSRGBDecode(bool decode) {
            AbstractTexture::setSRGBDecode(decode);
            return *this;
        }

        /**
         * @copybrief Texture::setSwizzle()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSwizzle() for more information.
         * @requires_gl33 Extension @extension{ARB,texture_swizzle}
         */
        template<char r, char g, char b, char a> TextureArray<dimensions>& setSwizzle() {
            AbstractTexture::setSwizzle<r, g, b, a>();
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setCompareMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareMode() for more information.
         */
        TextureArray<dimensions>& setCompareMode(Sampler::CompareMode mode) {
            AbstractTexture::setCompareMode(mode);
            return *this;
        }

        /**
         * @copybrief Texture::setCompareFunction()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareFunction() for more information.
         */
        TextureArray<dimensions>& setCompareFunction(Sampler::CompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @copybrief Texture::setDepthStencilMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setDepthStencilMode() for more information.
         * @requires_gl43 Extension @extension{ARB,stencil_texturing}
         * @requires_gles31 Stencil texturing is not available in OpenGL ES 3.0
         *      and older.
         * @requires_gles Stencil texturing is not available in WebGL.
         */
        TextureArray<dimensions>& setDepthStencilMode(Sampler::DepthStencilMode mode) {
            AbstractTexture::setDepthStencilMode(mode);
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setStorage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setStorage() for more information.
         * @see @ref maxSize()
         */
        TextureArray<dimensions>& setStorage(Int levels, TextureFormat internalFormat, const typename DimensionTraits<dimensions+1, Int>::VectorType& size) {
            DataHelper<dimensions+1>::setStorage(*this, levels, internalFormat, size);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @copybrief Texture::imageSize()
         *
         * See @ref Texture::imageSize() for more information.
         * @requires_gles31 Texture image size queries are not available in
         *      OpenGL ES 3.0 and older.
         * @requires_gles Texture image size queries are not available in
         *      WebGL.
         */
        typename DimensionTraits<dimensions+1, Int>::VectorType imageSize(Int level) {
            return DataHelper<dimensions+1>::imageSize(*this, level);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::image(Int, Image&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::image(Int, Image&) for more information.
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void image(Int level, Image<dimensions+1>& image) {
            AbstractTexture::image<dimensions+1>(level, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * Image3D image = texture.image(0, {ColorFormat::RGBA, ColorType::UnsignedByte});
         * @endcode
         */
        Image<dimensions+1> image(Int level, Image<dimensions+1>&& image);

        /**
         * @copybrief Texture::image(Int, BufferImage&, BufferUsage)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::image(Int, BufferImage&, BufferUsage) for more
         * information.
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void image(Int level, BufferImage<dimensions+1>& image, BufferUsage usage) {
            AbstractTexture::image<dimensions+1>(level, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * BufferImage3D image = texture.image(0, {ColorFormat::RGBA, ColorType::UnsignedByte}, BufferUsage::StaticRead);
         * @endcode
         */
        BufferImage<dimensions+1> image(Int level, BufferImage<dimensions+1>&& image, BufferUsage usage);

        /**
         * @copybrief Texture::subImage(Int, const typename DimensionTraits<dimensions, Int>::RangeType&, Image&)
         *
         * See @ref Texture::subImage(Int, const typename DimensionTraits<dimensions, Int>::RangeType&, Image&)
         * for more information.
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void subImage(Int level, const typename DimensionTraits<dimensions+1, Int>::RangeType& range, Image<dimensions+1>& image) {
            AbstractTexture::subImage<dimensions+1>(level, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * Image3D image = texture.subImage(0, range, {ColorFormat::RGBA, ColorType::UnsignedByte});
         * @endcode
         */
        Image<dimensions+1> subImage(Int level, const typename DimensionTraits<dimensions+1, Int>::RangeType& range, Image<dimensions+1>&& image);

        /**
         * @copybrief Texture::subImage(Int, const typename DimensionTraits<dimensions, Int>::RangeType&, BufferImage&, BufferUsage)
         *
         * See @ref Texture::subImage(Int, const typename DimensionTraits<dimensions, Int>::RangeType&, BufferImage&, BufferUsage)
         * for more information.
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void subImage(Int level, const typename DimensionTraits<dimensions+1, Int>::RangeType& range, BufferImage<dimensions+1>& image, BufferUsage usage) {
            AbstractTexture::subImage<dimensions+1>(level, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * BufferImage3D image = texture.subImage(0, range, {ColorFormat::RGBA, ColorType::UnsignedByte}, BufferUsage::StaticRead);
         * @endcode
         */
        BufferImage<dimensions+1> subImage(Int level, const typename DimensionTraits<dimensions+1, Int>::RangeType& range, BufferImage<dimensions+1>&& image, BufferUsage usage);
        #endif

        /**
         * @copybrief Texture::setImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setImage() for more information.
         * @see @ref maxSize()
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        TextureArray<dimensions>& setImage(Int level, TextureFormat internalFormat, const ImageReference<dimensions+1>& image) {
            DataHelper<dimensions+1>::setImage(*this, level, internalFormat, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        TextureArray<dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions+1>& image) {
            DataHelper<dimensions+1>::setImage(*this, level, internalFormat, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        TextureArray<dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions+1>&& image) {
            return setImage(level, internalFormat, image);
        }

        /**
         * @brief Set image subdata
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             @ref Image, @ref ImageReference or
         *      @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref setStorage(), @fn_gl2{TextureSubImage2D,TexSubImage2D}/
         *      @fn_gl2{TextureSubImage3D,TexSubImage3D},
         *      @fn_gl_extension{TextureSubImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureSubImage3D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexSubImage2D}/@fn_gl{TexSubImage3D}
         */
        TextureArray<dimensions>& setSubImage(Int level, const typename DimensionTraits<dimensions+1, Int>::VectorType& offset, const ImageReference<dimensions+1>& image) {
            DataHelper<dimensions+1>::setSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload */
        TextureArray<dimensions>& setSubImage(Int level, const typename DimensionTraits<dimensions+1, Int>::VectorType& offset, BufferImage<dimensions+1>& image) {
            DataHelper<dimensions+1>::setSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload */
        TextureArray<dimensions>& setSubImage(Int level, const typename DimensionTraits<dimensions+1, Int>::VectorType& offset, BufferImage<dimensions+1>&& image) {
            return setSubImage(level, offset, image);
        }

        /**
         * @copybrief Texture::generateMipmap()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::generateMipmap() for more information.
         * @requires_gl30 Extension @extension{ARB,framebuffer_object}
         */
        TextureArray<dimensions>& generateMipmap() {
            AbstractTexture::generateMipmap();
            return *this;
        }

        /**
         * @copybrief Texture::invalidateImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::invalidateImage() for more information.
         */
        void invalidateImage(Int level) { AbstractTexture::invalidateImage(level); }

        /**
         * @copybrief Texture::invalidateSubImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::invalidateSubImage() for more information.
         */
        void invalidateSubImage(Int level, const typename DimensionTraits<dimensions+1, Int>::VectorType& offset, const typename DimensionTraits<dimensions+1, Int>::VectorType& size) {
            DataHelper<dimensions+1>::invalidateSubImage(*this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(MAGNUM_TARGET_WEBGL)
        TextureArray<dimensions>& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        template<std::size_t size> TextureArray<dimensions>& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit TextureArray(GLuint id, ObjectFlags flags): AbstractTexture{id, Implementation::textureArrayTarget<dimensions>(), flags} {}
};

#ifndef MAGNUM_TARGET_GLES
/**
@brief One-dimensional texture array

@requires_gl30 Extension @extension{EXT,texture_array}
@requires_gl Only @ref Texture2DArray is available in OpenGL ES and WebGL.
*/
typedef TextureArray<1> Texture1DArray;
#endif

/**
@brief Two-dimensional texture array

@requires_gl30 Extension @extension{EXT,texture_array}
@requires_gles30 Array textures are not available in OpenGL ES 2.0.
@requires_webgl20 Array textures are not available in WebGL 1.0.
*/
typedef TextureArray<2> Texture2DArray;

}
#else
#error this header is not available in OpenGL ES 2.0 build
#endif

#endif
