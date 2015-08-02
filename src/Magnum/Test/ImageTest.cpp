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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/ColorFormat.h"
#include "Magnum/Image.h"

namespace Magnum { namespace Test {

struct ImageTest: TestSuite::Tester {
    explicit ImageTest();

    void construct();
    void constructCompressed();
    void constructCopy();
    void constructCopyCompressed();
    void constructMove();
    void constructMoveCompressed();

    void setData();
    void setDataCompressed();
    void toReference();
    void toReferenceCommpressed();
    void release();
    void releaseCompressed();
};

ImageTest::ImageTest() {
    addTests<ImageTest>({&ImageTest::construct,
              &ImageTest::constructCompressed,
              &ImageTest::constructCopy,
              &ImageTest::constructCopyCompressed,
              &ImageTest::constructMove,
              &ImageTest::constructMoveCompressed,

              &ImageTest::setData,
              &ImageTest::setDataCompressed,
              &ImageTest::toReference,
              &ImageTest::toReferenceCommpressed,
              &ImageTest::release,
              &ImageTest::releaseCompressed});
}

void ImageTest::construct() {
    auto data = new char[3];
    Image2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);

    CORRADE_COMPARE(a.format(), ColorFormat::Red);
    CORRADE_COMPARE(a.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), data);
}

void ImageTest::constructCompressed() {
    auto data = new char[8];
    CompressedImage2D a{CompressedColorFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};

    CORRADE_COMPARE(a.format(), CompressedColorFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.data(), data);
    CORRADE_COMPARE(a.data().size(), 8);
}

void ImageTest::constructCopy() {
    #ifndef CORRADE_GCC44_COMPATIBILITY
    {
        #ifdef CORRADE_MSVC2013_COMPATIBILITY
        CORRADE_EXPECT_FAIL("std::is_constructible is buggy on MSVC 2013.");
        #endif
        CORRADE_VERIFY(!(std::is_constructible<Image2D, const Image2D&>::value));
    }
    /* GCC 4.6 doesn't have std::is_assignable */
    #ifndef CORRADE_GCC46_COMPATIBILITY
    {
        #ifdef CORRADE_MSVC2013_COMPATIBILITY
        CORRADE_EXPECT_FAIL("std::is_assignable is buggy on MSVC 2013.");
        #endif
        CORRADE_VERIFY(!(std::is_assignable<Image2D, const Image2D&>{}));
    }
    #endif
    #else
    CORRADE_SKIP("Type traits needed to test this are not available on GCC 4.4.");
    #endif
}

void ImageTest::constructCopyCompressed() {
    CORRADE_VERIFY(!(std::is_constructible<CompressedImage2D, const CompressedImage2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<CompressedImage2D, const CompressedImage2D&>{}));
}

void ImageTest::constructMove() {
    auto data = new char[3];
    Image2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);
    Image2D b(std::move(a));

    #ifndef CORRADE_GCC45_COMPATIBILITY
    CORRADE_COMPARE(a.data(), nullptr);
    #else
    CORRADE_VERIFY(a.data() == nullptr);
    #endif
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_COMPARE(b.format(), ColorFormat::Red);
    CORRADE_COMPARE(b.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);

    auto data2 = new char[12*4*2];
    Image2D c(ColorFormat::RGBA, ColorType::UnsignedShort, {2, 6}, data2);
    c = std::move(b);

    CORRADE_COMPARE(b.data(), data2);
    CORRADE_COMPARE(b.size(), Vector2i(2, 6));

    CORRADE_COMPARE(c.format(), ColorFormat::Red);
    CORRADE_COMPARE(c.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(c.size(), Vector2i(1, 3));
    CORRADE_COMPARE(c.data(), data);
}

void ImageTest::constructMoveCompressed() {
    auto data = new char[8];
    CompressedImage2D a{CompressedColorFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    CompressedImage2D b{std::move(a)};

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_COMPARE(b.format(), CompressedColorFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(b.size(), Vector2i(4, 4));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);

    auto data2 = new char[16];
    CompressedImage2D c{CompressedColorFormat::RGBAS3tcDxt3, {8, 4}, Containers::Array<char>{data2, 16}};
    c = std::move(b);

    CORRADE_COMPARE_AS(b.data(), data2, char*);
    CORRADE_COMPARE(b.data().size(), 16);
    CORRADE_COMPARE(b.size(), Vector2i(8, 4));

    CORRADE_COMPARE(c.format(), CompressedColorFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(c.size(), Vector2i(4, 4));
    CORRADE_COMPARE(c.data(), data);
    CORRADE_COMPARE(c.data().size(), 8);
}

void ImageTest::setData() {
    auto data = new char[3];
    Image2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);
    auto data2 = new char[2*4];
    a.setData(ColorFormat::RGBA, ColorType::UnsignedShort, {2, 1}, data2);

    CORRADE_COMPARE(a.format(), ColorFormat::RGBA);
    CORRADE_COMPARE(a.type(), ColorType::UnsignedShort);
    CORRADE_COMPARE(a.size(), Vector2i(2, 1));
    CORRADE_COMPARE(a.data(), data2);
}

void ImageTest::setDataCompressed() {
    auto data = new char[8];
    CompressedImage2D a{CompressedColorFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    auto data2 = new char[16];
    a.setData(CompressedColorFormat::RGBAS3tcDxt3, {8, 4}, Containers::Array<char>{data2, 16});

    CORRADE_COMPARE(a.format(), CompressedColorFormat::RGBAS3tcDxt3);
    CORRADE_COMPARE(a.size(), Vector2i(8, 4));
    CORRADE_COMPARE(a.data(), data2);
    CORRADE_COMPARE(a.data().size(), 16);
}

void ImageTest::toReference() {
    auto data = new char[3];
    const Image2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);
    ImageView2D b = a;

    CORRADE_COMPARE(b.format(), ColorFormat::Red);
    CORRADE_COMPARE(b.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);

    CORRADE_VERIFY((std::is_convertible<const Image2D&, ImageView2D>::value));
    {
        #if defined(CORRADE_GCC47_COMPATIBILITY) || defined(CORRADE_MSVC2013_COMPATIBILITY)
        CORRADE_EXPECT_FAIL("Rvalue references for *this are not supported in GCC < 4.8.1 and MSVC 2013.");
        #endif
        CORRADE_VERIFY(!(std::is_convertible<const Image2D, ImageView2D>::value));
        CORRADE_VERIFY(!(std::is_convertible<const Image2D&&, ImageView2D>::value));
    }
}

void ImageTest::toReferenceCommpressed() {
    auto data = new char[8];
    const CompressedImage2D a{CompressedColorFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    CompressedImageView2D b = a;

    CORRADE_COMPARE(b.format(), CompressedColorFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(b.size(), Vector2i(4, 4));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);

    CORRADE_VERIFY((std::is_convertible<const CompressedImage2D&, CompressedImageView2D>::value));
    {
        #ifdef CORRADE_GCC47_COMPATIBILITY
        CORRADE_EXPECT_FAIL("Rvalue references for *this are not supported in GCC < 4.8.1.");
        #endif
        CORRADE_VERIFY(!(std::is_convertible<const CompressedImage2D, CompressedImageView2D>::value));
        CORRADE_VERIFY(!(std::is_convertible<const CompressedImage2D&&, CompressedImageView2D>::value));
    }
}

void ImageTest::release() {
    char data[] = {'c', 'a', 'f', 'e'};
    Image2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 4}, data);
    const char* const pointer = a.release();

    CORRADE_COMPARE(pointer, data);
    #ifndef CORRADE_GCC45_COMPATIBILITY
    CORRADE_COMPARE(a.data(), nullptr);
    #else
    CORRADE_VERIFY(a.data() == nullptr);
    #endif
    CORRADE_COMPARE(a.size(), Vector2i());
}

void ImageTest::releaseCompressed() {
    char data[8];
    CompressedImage2D a{CompressedColorFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    const char* const pointer = a.release().release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());
}

}}

CORRADE_TEST_MAIN(Magnum::Test::ImageTest)
