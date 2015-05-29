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

#include "TgaImageConverter.h"

#include <algorithm>
#include <fstream>
#include <tuple>
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/ColorFormat.h"
#include "Magnum/Image.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Vector4.h"
#include "MagnumPlugins/TgaImporter/TgaHeader.h"

namespace Magnum { namespace Trade {

namespace {
    constexpr Math::Vector3<UnsignedByte> bgr(const Math::Vector3<UnsignedByte>& vec) {
        return Math::swizzle<'b', 'g', 'r'>(vec);
    }

    constexpr Math::Vector4<UnsignedByte> bgra(const Math::Vector4<UnsignedByte>& vec) {
        return Math::swizzle<'b', 'g', 'r', 'a'>(vec);
    }
}

TgaImageConverter::TgaImageConverter() = default;

TgaImageConverter::TgaImageConverter(PluginManager::AbstractManager& manager, std::string plugin): AbstractImageConverter(manager, std::move(plugin)) {}

auto TgaImageConverter::doFeatures() const -> Features { return Feature::ConvertData; }

Containers::Array<char> TgaImageConverter::doExportToData(const ImageReference2D& image) const {
    if(image.format() != ColorFormat::RGB &&
       image.format() != ColorFormat::RGBA
       #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
       && image.format() != ColorFormat::Red
       #endif
       #ifdef MAGNUM_TARGET_GLES2
       && image.format() != ColorFormat::Luminance
       #endif
       )
    {
        Error() << "Trade::TgaImageConverter::exportToData(): unsupported color format" << image.format();
        return {};
    }

    if(image.type() != ColorType::UnsignedByte) {
        Error() << "Trade::TgaImageConverter::exportToData(): unsupported color type" << image.type();
        return {};
    }

    /* Initialize data buffer */
    const auto pixelSize = UnsignedByte(image.pixelSize());
    auto data = Containers::Array<char>::zeroInitialized(sizeof(TgaHeader) + pixelSize*image.size().product());

    /* Fill header */
    auto header = reinterpret_cast<TgaHeader*>(data.begin());
    switch(image.format()) {
        case ColorFormat::RGB:
        case ColorFormat::RGBA:
            header->imageType = 2;
            break;
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case ColorFormat::Red:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case ColorFormat::Luminance:
        #endif
            header->imageType = 3;
            break;
        default: CORRADE_ASSERT_UNREACHABLE();
    }
    header->bpp = pixelSize*8;
    header->width = UnsignedShort(Utility::Endianness::littleEndian(image.size().x()));
    header->height = UnsignedShort(Utility::Endianness::littleEndian(image.size().y()));

    /* Fill data */
    std::copy(image.data(), image.data()+pixelSize*image.size().product(), data.begin()+sizeof(TgaHeader));

    if(image.format() == ColorFormat::RGB) {
        auto pixels = reinterpret_cast<Math::Vector3<UnsignedByte>*>(data.begin()+sizeof(TgaHeader));
        std::transform(pixels, pixels + image.size().product(), pixels, bgr);
    } else if(image.format() == ColorFormat::RGBA) {
        auto pixels = reinterpret_cast<Math::Vector4<UnsignedByte>*>(data.begin()+sizeof(TgaHeader));
        std::transform(pixels, pixels + image.size().product(), pixels, bgra);
    }

    return data;
}

}}
