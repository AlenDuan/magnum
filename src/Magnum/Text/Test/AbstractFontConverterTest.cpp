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

#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/FileToString.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Text/AbstractFontConverter.h"
#include "Magnum/Text/GlyphCache.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test {

struct AbstractFontConverterTest: TestSuite::Tester {
    explicit AbstractFontConverterTest();

    void convertGlyphs();

    void exportFontToSingleData();
    void exportFontToFile();

    void exportGlyphCacheToSingleData();
    void exportGlyphCacheToFile();

    void importGlyphCacheFromSingleData();
    void importGlyphCacheFromFile();
};

AbstractFontConverterTest::AbstractFontConverterTest() {
    addTests<AbstractFontConverterTest>({&AbstractFontConverterTest::convertGlyphs,

              &AbstractFontConverterTest::exportFontToSingleData,
              &AbstractFontConverterTest::exportFontToFile,

              &AbstractFontConverterTest::exportGlyphCacheToSingleData,
              &AbstractFontConverterTest::exportGlyphCacheToFile,

              &AbstractFontConverterTest::importGlyphCacheFromSingleData,
              &AbstractFontConverterTest::importGlyphCacheFromFile});
}

namespace {
    /* *static_cast<GlyphCache*>(nullptr) makes Clang Analyzer grumpy */
    char nullData;
    AbstractFont& nullFont = *reinterpret_cast<AbstractFont*>(nullData);
    GlyphCache& nullGlyphCache = *reinterpret_cast<GlyphCache*>(nullData);
}

void AbstractFontConverterTest::convertGlyphs() {
    class GlyphExporter: public AbstractFontConverter {
        public:
            #ifndef __MINGW32__
            GlyphExporter(std::u32string& characters): characters(characters) {}
            #else
            GlyphExporter(std::vector<char32_t>& characters): characters(characters) {}
            #endif

        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportFont; }

            #ifndef __MINGW32__
            Containers::Array<char> doExportFontToSingleData(AbstractFont&, GlyphCache&, const std::u32string& characters) const override
            #else
            Containers::Array<char> doExportFontToSingleData(AbstractFont&, GlyphCache&, const std::vector<char32_t>& characters) const override
            #endif
            {
                this->characters = characters;
                return {};
            }

            #ifndef __MINGW32__
            std::u32string& characters;
            #else
            std::vector<char32_t>& characters;
            #endif
    };

    #ifndef __MINGW32__
    std::u32string characters;
    #else
    std::vector<char32_t> characters;
    #endif
    GlyphExporter exporter(characters);
    exporter.exportFontToSingleData(nullFont, nullGlyphCache, "abC01a0 ");
    #if !defined(__MINGW32__) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    CORRADE_COMPARE(characters, U" 01Cab");
    #elif defined(CORRADE_MSVC2013_COMPATIBILITY)
    CORRADE_COMPARE(characters, (std::u32string{
            ' ', '0', '1', 'C', 'a', 'b'}));
    #else
    CORRADE_COMPARE(characters, (std::vector<char32_t>{
            U' ', U'0', U'1', U'C', U'a', U'b'}));
    #endif
}

void AbstractFontConverterTest::exportFontToSingleData() {
    class SingleDataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportFont; }

            #ifndef __MINGW32__
            Containers::Array<char> doExportFontToSingleData(AbstractFont&, GlyphCache&, const std::u32string&) const override
            #else
            Containers::Array<char> doExportFontToSingleData(AbstractFont&, GlyphCache&, const std::vector<char32_t>&) const override
            #endif
            {
                Containers::Array<char> data(1);
                data[0] = '\xee';
                return data;
            }
    };

    /* doExportFontToData() should call doExportFontToSingleData() */
    SingleDataExporter exporter;
    auto ret = exporter.exportFontToData(nullFont, nullGlyphCache, "font.out", {});
    CORRADE_COMPARE(ret.size(), 1);
    CORRADE_COMPARE(ret[0].first, "font.out");
    CORRADE_COMPARE(ret[0].second.size(), 1);
    CORRADE_COMPARE(ret[0].second[0], '\xee');
}

void AbstractFontConverterTest::exportFontToFile() {
    class DataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportFont|Feature::MultiFile; }

            #ifndef __MINGW32__
            std::vector<std::pair<std::string, Containers::Array<char>>> doExportFontToData(AbstractFont&, GlyphCache&, const std::string& filename, const std::u32string&) const override
            #else
            std::vector<std::pair<std::string, Containers::Array<char>>> doExportFontToData(AbstractFont&, GlyphCache&, const std::string& filename, const std::vector<char32_t>&) const override
            #endif
            {
                /* Why the hell GCC 4.9 fails to do proper move so I need to
                   work around that this ugly way?! */
                std::vector<std::pair<std::string, Containers::Array<char>>> ret;
                ret.emplace_back(filename, Containers::Array<char>::from('\xf0'));
                ret.emplace_back(filename + ".data", Containers::Array<char>::from('\xfe', '\xed'));
                return ret;
            }
    };

    /* Remove previous files */
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out"));
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out.data"));

    /* doExportToFile() should call doExportToData() */
    DataExporter exporter;
    bool exported = exporter.exportFontToFile(nullFont, nullGlyphCache, Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out"), {});
    CORRADE_VERIFY(exported);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out"),
                       "\xf0", TestSuite::Compare::FileToString);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out.data"),
                       "\xfe\xed", TestSuite::Compare::FileToString);
}

void AbstractFontConverterTest::exportGlyphCacheToSingleData() {
    class SingleDataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportGlyphCache; }

            Containers::Array<char> doExportGlyphCacheToSingleData(GlyphCache&) const override {
                return Containers::Array<char>::from('\xee');
            }
    };

    /* doExportGlyphCacheToData() should call doExportGlyphCacheToSingleData() */
    SingleDataExporter exporter;
    auto ret = exporter.exportGlyphCacheToData(nullGlyphCache, "font.out");
    CORRADE_COMPARE(ret.size(), 1);
    CORRADE_COMPARE(ret[0].first, "font.out");
    CORRADE_COMPARE_AS(ret[0].second, Containers::Array<char>::from('\xee'), TestSuite::Compare::Container);
}

void AbstractFontConverterTest::exportGlyphCacheToFile() {
    class DataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportGlyphCache|Feature::MultiFile; }

            std::vector<std::pair<std::string, Containers::Array<char>>> doExportGlyphCacheToData(GlyphCache&, const std::string& filename) const override {
                /* Why the hell GCC 4.9 fails to do proper move so I need to
                   work around that this ugly way?! */
                std::vector<std::pair<std::string, Containers::Array<char>>> ret;
                ret.emplace_back(filename, Containers::Array<char>::from('\xf0'));
                ret.emplace_back(filename + ".data", Containers::Array<char>::from('\xfe', '\xed'));
                return ret;
            }
    };

    /* Remove previous files */
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out"));
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out.data"));

    /* doExportGlyphCacheToFile() should call doExportGlyphCacheToData() */
    DataExporter exporter;
    bool exported = exporter.exportGlyphCacheToFile(nullGlyphCache, Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out"));
    CORRADE_VERIFY(exported);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out"),
                       "\xf0", TestSuite::Compare::FileToString);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out.data"),
                       "\xfe\xed", TestSuite::Compare::FileToString);
}

namespace {

class SingleGlyphCacheDataImporter: public Text::AbstractFontConverter {
    private:
        Features doFeatures() const override { return Feature::ConvertData|Feature::ImportGlyphCache; }

        std::unique_ptr<GlyphCache> doImportGlyphCacheFromSingleData(const Containers::ArrayReference<const char> data) const override {
            if(data.size() == 1 && data[0] == '\xa5')
                return std::unique_ptr<GlyphCache>(reinterpret_cast<GlyphCache*>(0xdeadbeef));
            return {};
        }
};

}

void AbstractFontConverterTest::importGlyphCacheFromSingleData() {
    /* doImportFromData() should call doImportFromSingleData() */
    SingleGlyphCacheDataImporter importer;
    const char data[] = {'\xa5'};
    /* GCC 4.4 needs explicit typing */
    std::unique_ptr<GlyphCache> cache = importer.importGlyphCacheFromData(std::vector<std::pair<std::string, Containers::ArrayReference<const char>>>{{{}, data}});
    CORRADE_COMPARE(cache.get(), reinterpret_cast<GlyphCache*>(0xdeadbeef));

    /* The pointer is invalid, avoid deletion */
    cache.release();
}

void AbstractFontConverterTest::importGlyphCacheFromFile() {
    /* doImportFromFile() should call doImportFromSingleData() */
    SingleGlyphCacheDataImporter importer;
    std::unique_ptr<GlyphCache> cache = importer.importGlyphCacheFromFile(Utility::Directory::join(TEXT_TEST_DIR, "data.bin"));
    CORRADE_COMPARE(cache.get(), reinterpret_cast<GlyphCache*>(0xdeadbeef));

    /* The pointer is invalid, avoid deletion */
    cache.release();
}

}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractFontConverterTest)
