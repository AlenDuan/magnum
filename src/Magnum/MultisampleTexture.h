#ifndef Magnum_MultisampleTexture_h
#define Magnum_MultisampleTexture_h
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

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/** @file
 * @brief Class @ref Magnum::MultisampleTexture, typedef @ref Magnum::MultisampleTexture2D, @ref Magnum::MultisampleTexture2DArray
 */
#endif

#include "Magnum/AbstractTexture.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
namespace Magnum {

namespace Implementation {
    template<UnsignedInt> constexpr GLenum multisampleTextureTarget();
    /* GCC 4.5 doesn't have constexpr, so it needs inline */
    template<> constexpr inline GLenum multisampleTextureTarget<2>() { return GL_TEXTURE_2D_MULTISAMPLE; }
    #ifndef MAGNUM_TARGET_GLES
    template<> constexpr inline GLenum multisampleTextureTarget<3>() { return GL_TEXTURE_2D_MULTISAMPLE_ARRAY; }
    #endif

    template<UnsignedInt dimensions> typename DimensionTraits<dimensions, Int>::VectorType maxMultisampleTextureSize();
    template<> MAGNUM_EXPORT Vector2i maxMultisampleTextureSize<2>();
    #ifndef MAGNUM_TARGET_GLES
    template<> MAGNUM_EXPORT Vector3i maxMultisampleTextureSize<3>();
    #endif
}

/**
@brief Multisample texture sample locations

@see @ref MultisampleTexture::setStorage()
*/
enum class MultisampleTextureSampleLocations: GLboolean {
    NotFixed = GL_FALSE,
    Fixed = GL_TRUE
};

/**
@brief Mulitsample texture

Template class for 2D mulitsample texture and 2D multisample texture array.
Used only from shaders for manual multisample resolve and other operations. See
also @ref AbstractTexture documentation for more information.

## Usage

As multisample textures have no sampler state, the only thing you need is to
set storage:
@code
MultisampleTexture2D texture;
texture.setStorage(16, TextureFormat::RGBA8, {1024, 1024});
@endcode

In shader, the texture is used via `sampler2DMS`/`sampler2DMSArray`,
`isampler2DMS`/`isampler2DMSArray` or `usampler2DMS`/`usampler2DMSArray`. See
@ref AbstractShaderProgram documentation for more information about usage in
shaders.

@see @ref MultisampleTexture2D, @ref MultisampleTexture2DArray, @ref Texture,
    @ref TextureArray, @ref CubeMapTexture, @ref CubeMapTextureArray,
    @ref RectangleTexture, @ref BufferTexture
@requires_gl32 Extension @extension{ARB,texture_multisample}
@requires_gles31 Multisample textures are not available in OpenGL ES 3.0 and
    older.
@requires_gl 2D array multisample textures are not available in OpenGL ES, only
    2D ones. No multisample textures are available in WebGL.
@requires_gles Multisample textures are not available in WebGL.
 */
template<UnsignedInt dimensions> class MultisampleTexture: public AbstractTexture {
    public:
        static const UnsignedInt Dimensions = dimensions; /**< @brief Texture dimension count */

        /**
         * @brief Max supported multisample texture size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,texture_multisample}
         * (part of OpenGL 3.2) nor OpenGL ES 3.1 is available, returns zero
         * vector.
         * @see @fn_gl{Get} with @def_gl{MAX_TEXTURE_SIZE} and
         *      @def_gl{MAX_3D_TEXTURE_SIZE}
         */
        static typename DimensionTraits<dimensions, Int>::VectorType maxSize() {
            return Implementation::maxMultisampleTextureSize<dimensions>();
        }

        /**
         * @brief Wrap existing OpenGL multisample texture object
         * @param id            OpenGL multisample texture ID
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL texture object
         * with target @def_gl{TEXTURE_2D_MULTISAMPLE} or
         * @def_gl{TEXTURE_2D_MULTISAMPLE_ARRAY} based on dimension count.
         * Unlike texture created using constructor, the OpenGL object is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static MultisampleTexture<dimensions> wrap(GLuint id, ObjectFlags flags = {}) {
            return MultisampleTexture<dimensions>{id, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref wrap(), @fn_gl{CreateTextures} with
         *      @def_gl{TEXTURE_2D_MULTISAMPLE} or
         *      @def_gl{TEXTURE_2D_MULTISAMPLE_ARRAY}, eventually
         *      @fn_gl{GenTextures}
         */
        explicit MultisampleTexture(): AbstractTexture(Implementation::multisampleTextureTarget<dimensions>()) {}

        /**
         * @brief Set storage
         * @param samples           Sample count
         * @param internalFormat    Internal format
         * @param size              Texture size
         * @param sampleLocations   Whether to use fixed sample locations
         * @return Reference to self (for method chaining)
         *
         * After calling this function the texture is immutable and calling
         * @ref setStorage() again is not allowed.
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already). If @extension{ARB,texture_storage_multisample} (part of
         * OpenGL 4.3) is not available, the texture is bound and the feature
         * is emulated using plain @extension{ARB,texture_multisample}
         * functionality.
         * @see @ref maxSize(), @ref maxColorSamples(), @ref maxDepthSamples(),
         *      @ref maxIntegerSamples(), @fn_gl2{TextureStorage2DMultisample,TexStorage2DMultisample} /
         *      @fn_gl2{TextureStorage3DMultisample,TexStorage3DMultisample},
         *      @fn_gl_extension{TextureStorage2DMultisample,EXT,direct_state_access} /
         *      @fn_gl_extension{TextureStorage3DMultisample,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture}
         *      and @fn_gl{TexStorage2DMultisample} / @fn_gl{TexStorage3DMultisample}
         *      or @fn_gl{TexImage2DMultisample} / @fn_gl{TexImage3DMultisample}
         * @todoc Remove the workaround when it stops breaking Doxygen layout so badly
         */
        /* The default parameter value was chosen based on discussion in
           ARB_texture_multisample specs (fixed locations is treated as the
           special case) */
        MultisampleTexture<dimensions>& setStorage(Int samples, TextureFormat internalFormat, const typename DimensionTraits<dimensions, Int>::VectorType& size, MultisampleTextureSampleLocations sampleLocations =
            #ifndef DOXYGEN_GENERATING_OUTPUT
            MultisampleTextureSampleLocations::NotFixed
            #else
            NotFixed
            #endif
        ) {
            DataHelper<dimensions>::setStorageMultisample(*this, samples, internalFormat, size, GLboolean(sampleLocations));
            return *this;
        }

        /**
         * @brief Texture image size
         *
         * See @ref Texture::imageSize() for more information.
         * @requires_gles31 Texture image size queries are not available in
         *      OpenGL ES 3.0 and older.
         */
        typename DimensionTraits<dimensions, Int>::VectorType imageSize() {
            return DataHelper<dimensions>::imageSize(*this, 0);
        }

        /**
         * @copybrief Texture::invalidateImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::invalidateImage() for more information.
         */
        void invalidateImage() { AbstractTexture::invalidateImage(0); }

        /**
         * @copybrief Texture::invalidateSubImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::invalidateSubImage() for more information.
         */
        void invalidateSubImage(const typename DimensionTraits<dimensions, Int>::VectorType& offset, const typename DimensionTraits<dimensions, Int>::VectorType& size) {
            DataHelper<dimensions>::invalidateSubImage(*this, 0, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        MultisampleTexture<dimensions>& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        template<std::size_t size> MultisampleTexture<dimensions>& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit MultisampleTexture(GLuint id, ObjectFlags flags): AbstractTexture{id, Implementation::multisampleTextureTarget<dimensions>(), flags} {}
};

/**
@brief Two-dimensional multisample texture

@requires_gl32 Extension @extension{ARB,texture_multisample}
@requires_gles31 Multisample textures are not available in OpenGL ES 3.0 and
    older.
@requires_gles Multisample textures are not available in WebGL.
*/
typedef MultisampleTexture<2> MultisampleTexture2D;

#ifndef MAGNUM_TARGET_GLES
/**
@brief Two-dimensional multisample texture array

@requires_gl32 Extension @extension{ARB,texture_multisample}
@requires_gl Only @ref MultisampleTexture2D is available in OpenGL ES.
    No multisample textures are available in WebGL.
*/
typedef MultisampleTexture<3> MultisampleTexture2DArray;
#endif

}
#else
#error this header is not available in OpenGL ES 2.0 and WebGL build
#endif

#endif
