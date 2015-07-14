#ifndef Magnum_Trade_ImageData_h
#define Magnum_Trade_ImageData_h
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
 * @brief Class @ref Magnum::Trade::ImageData, typedef @ref Magnum::Trade::ImageData1D, @ref Magnum::Trade::ImageData2D, @ref Magnum::Trade::ImageData3D
 */

#include "Magnum/ImageReference.h"

namespace Magnum { namespace Trade {

/**
@brief Image data

Access to image data provided by @ref AbstractImporter subclasses.
Interchangeable with @ref Image, @ref ImageReference or @ref BufferImage.
@see @ref ImageData1D, @ref ImageData2D, @ref ImageData3D
*/
template<UnsignedInt dimensions> class ImageData: public AbstractImage {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        explicit ImageData(ColorFormat format, ColorType type, const typename DimensionTraits<Dimensions, Int>::VectorType& size, void* data): AbstractImage{format, type}, _size{size}, _data{reinterpret_cast<char*>(data)} {}

        /** @brief Copying is not allowed */
        ImageData(const ImageData<dimensions>&) = delete;

        /** @brief Move constructor */
        ImageData(ImageData<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        ImageData<dimensions>& operator=(const ImageData<dimensions>&) = delete;

        /** @brief Move assignment */
        ImageData<dimensions>& operator=(ImageData<dimensions>&& other) noexcept;

        /** @brief Destructor */
        ~ImageData() { delete[] _data; }

        /** @brief Conversion to reference */
        /*implicit*/ operator ImageReference<dimensions>()
        #if !defined(CORRADE_GCC47_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
        const &;
        #else
        const;
        #endif

        #if !defined(CORRADE_GCC47_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
        /** @overload */
        /*implicit*/ operator ImageReference<dimensions>() const && = delete;
        #endif

        /** @brief Image size */
        typename DimensionTraits<Dimensions, Int>::VectorType size() const { return _size; }

        /** @copydoc Image::dataSize() */
        std::size_t dataSize(const typename DimensionTraits<Dimensions, Int>::VectorType& size) const {
            return AbstractImage::dataSize<dimensions>(size);
        }

        /**
         * @brief Pointer to raw data
         *
         * @see @ref release()
         */
        template<class T = char> T* data() {
            return reinterpret_cast<T*>(_data);
        }

        /** @overload */
        template<class T = char> const T* data() const {
            return reinterpret_cast<const T*>(_data);
        }

        /**
         * @brief Release data storage
         *
         * Releases the ownership of the data pointer and resets internal state
         * to default. Deleting the returned array is then user responsibility.
         * @see @ref data()
         */
        char* release();

    private:
        Math::Vector<Dimensions, Int> _size;
        char* _data;
};

/** @brief One-dimensional image */
typedef ImageData<1> ImageData1D;

/** @brief Two-dimensional image */
typedef ImageData<2> ImageData2D;

/** @brief Three-dimensional image */
typedef ImageData<3> ImageData3D;

template<UnsignedInt dimensions> inline ImageData<dimensions>::ImageData(ImageData<dimensions>&& other) noexcept: AbstractImage(std::move(other)), _size(std::move(other._size)), _data(std::move(other._data)) {
    other._size = {};
    other._data = nullptr;
}

template<UnsignedInt dimensions> inline ImageData<dimensions>& ImageData<dimensions>::operator=(ImageData<dimensions>&& other) noexcept {
    AbstractImage::operator=(std::move(other));
    using std::swap;
    swap(_size, other._size);
    swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> inline ImageData<dimensions>::operator ImageReference<dimensions>()
#if !defined(CORRADE_GCC47_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
const &
#else
const
#endif
{
    return ImageReference<dimensions>(AbstractImage::format(), AbstractImage::type(), _size, _data);
}

template<UnsignedInt dimensions> inline char* ImageData<dimensions>::release() {
    /** @todo I need `std::exchange` NOW. */
    char* const data = _data;
    _size = {};
    _data = nullptr;
    return data;
}

}}

#endif
