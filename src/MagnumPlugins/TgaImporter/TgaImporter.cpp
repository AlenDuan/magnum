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

#include "TgaImporter.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <Corrade/Utility/Endianness.h>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/ColorFormat.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Trade/ImageData.h"
#include "MagnumPlugins/TgaImporter/TgaHeader.h"

#ifdef MAGNUM_TARGET_GLES2
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#endif

namespace Magnum { namespace Trade {

namespace {
    constexpr Math::Vector3<UnsignedByte> bgr(const Math::Vector3<UnsignedByte>& vec) {
        return Math::swizzle<'b', 'g', 'r'>(vec);
    }

    constexpr Math::Vector4<UnsignedByte> bgra(const Math::Vector4<UnsignedByte>& vec) {
        return Math::swizzle<'b', 'g', 'r', 'a'>(vec);
    }
}

TgaImporter::TgaImporter(): in(nullptr) {}

TgaImporter::TgaImporter(PluginManager::AbstractManager& manager, std::string plugin): AbstractImporter(manager, std::move(plugin)), in(nullptr) {}

TgaImporter::~TgaImporter() { close(); }

auto TgaImporter::doFeatures() const -> Features { return Feature::OpenData; }

bool TgaImporter::doIsOpened() const { return in; }

void TgaImporter::doOpenData(const Containers::ArrayView<const char> data) {
    /* GCC 4.5 can't handle {} here */
    in = new std::istringstream{std::string(data, data.size())};
}

void TgaImporter::doOpenFile(const std::string& filename) {
    in = new std::ifstream(filename, std::ifstream::binary);
    if(in->good()) return;

    Error() << "Trade::TgaImporter::openFile(): cannot open file" << filename;
    close();
}

void TgaImporter::doClose() {
    delete in;
    in = nullptr;
}

UnsignedInt TgaImporter::doImage2DCount() const { return 1; }

std::optional<ImageData2D> TgaImporter::doImage2D(UnsignedInt) {
    /* Check if the file is long enough */
    in->seekg(0, std::istream::end);
    std::streamoff filesize = in->tellg();
    in->seekg(0, std::istream::beg);
    if(filesize < std::streamoff(sizeof(TgaHeader))) {
        Error() << "Trade::TgaImporter::image2D(): the file is too short:" << filesize << "bytes";
        return std::nullopt;
    }

    TgaHeader header;
    in->read(reinterpret_cast<char*>(&header), sizeof(TgaHeader));

    /* Convert to machine endian */
    header.width = Utility::Endianness::littleEndian(header.width);
    header.height = Utility::Endianness::littleEndian(header.height);

    /* Image format */
    ColorFormat format;
    if(header.colorMapType != 0) {
        Error() << "Trade::TgaImporter::image2D(): paletted files are not supported";
        return std::nullopt;
    }

    /* Color */
    if(header.imageType == 2) {
        switch(header.bpp) {
            case 24:
                format = ColorFormat::RGB;
                break;
            case 32:
                format = ColorFormat::RGBA;
                break;
            default:
                Error() << "Trade::TgaImporter::image2D(): unsupported color bits-per-pixel:" << header.bpp;
                return std::nullopt;
        }

    /* Grayscale */
    } else if(header.imageType == 3) {
        #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        format = Context::current() && Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_rg>() ?
            ColorFormat::Red : ColorFormat::Luminance;
        #elif !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        format = ColorFormat::Red;
        #else
        format = ColorFormat::Luminance;
        #endif
        if(header.bpp != 8) {
            Error() << "Trade::TgaImporter::image2D(): unsupported grayscale bits-per-pixel:" << header.bpp;
            return std::nullopt;
        }

    /* Compressed files */
    } else {
        Error() << "Trade::TgaImporter::image2D(): unsupported (compressed?) image type:" << header.imageType;
        return std::nullopt;
    }

    const std::size_t dataSize = header.width*header.height*header.bpp/8;
    char* const data = new char[dataSize];
    in->read(data, dataSize);

    Vector2i size(header.width, header.height);

    if(format == ColorFormat::RGB) {
        auto pixels = reinterpret_cast<Math::Vector3<UnsignedByte>*>(data);
        std::transform(pixels, pixels + size.product(), pixels, bgr);
    } else if(format == ColorFormat::RGBA) {
        auto pixels = reinterpret_cast<Math::Vector4<UnsignedByte>*>(data);
        std::transform(pixels, pixels + size.product(), pixels, bgra);
    }

    return ImageData2D(format, ColorType::UnsignedByte, size, data);
}

}}
