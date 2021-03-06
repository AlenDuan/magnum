/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include <Corrade/Utility/Arguments.h>

#include "Magnum/Magnum.h"
#include "Magnum/Audio/Context.h"

namespace Magnum {

/** @page magnum-al-info Magnum OpenAL Info
@brief Displays information about Magnum OpenAL capabilities

@section magnum-al-info-usage Usage

    magnum-al-info [-h|--help] [-s|--short] [--extension-strings]

Arguments:
-   `-h`,` --help` -- display this help message and exit
-   `-s`, `--short` -- display just essential info and exit
-   `--extension-strings` -- list all extension strings provided by the driver
    (implies `--short`)

@section magnum-al-info-example Example output

```
  +---------------------------------------------------------+
  | Information about Magnum engine and OpenAL capabilities |
  +---------------------------------------------------------+

connect(2) call to /dev/shm/jack-1000/default/jack_0 failed (err=No such file or directory)
attempt to connect to server failed
Audio Renderer: OpenAL Soft by OpenAL Community
OpenAL version: 1.1 ALSOFT 1.17.2
Available devices:
    OpenAL Soft
Current device: OpenAL Soft
Vendor extension support:
    AL_EXT_FLOAT32                                                SUPPORTED
    AL_EXT_DOUBLE                                                 SUPPORTED
    AL_EXT_ALAW                                                   SUPPORTED
    AL_EXT_MULAW                                                  SUPPORTED
    AL_EXT_MCFORMATS                                              SUPPORTED
    ALC_ENUMERATION_EXT                                           SUPPORTED
    ALC_SOFTX_HRTF                                                   -
    ALC_SOFT_HRTF                                                 SUPPORTED

```

*/

}

using namespace Magnum;

int main(const int argc, const char** const argv) {
    Utility::Arguments args;
    args.addBooleanOption('s', "short").setHelp("short", "display just essential info and exit")
        .addBooleanOption("extension-strings").setHelp("extension-strings", "list all extension strings provided by the driver (implies --short)")
        .parse(argc, argv);

    Debug() << "";
    Debug() << "  +---------------------------------------------------------+";
    Debug() << "  | Information about Magnum engine and OpenAL capabilities |";
    Debug() << "  +---------------------------------------------------------+";
    Debug() << "";

    Audio::Context c;
    Debug() << "Available devices:";
    for(const auto& device: Audio::Context::deviceSpecifierStrings())
        Debug() << "   " << device;
    Debug() << "Current device:" << c.deviceSpecifierString();

    if(args.isSet("extension-strings")) {
        Debug() << "Extension strings:" << Debug::newline
            << c.extensionStrings();
        return 0;
    }

    if(args.isSet("short")) return 0;

    Debug() << "Vendor extension support:";
    for(const auto& extension: Audio::Extension::extensions()) {
        std::string extensionName = extension.string();
        Debug d;
        d << "   " << extensionName << std::string(60-extensionName.size(), ' ');
        if(c.isExtensionSupported(extension))
            d << "SUPPORTED";
        else
            d << "   -";
    }
}
