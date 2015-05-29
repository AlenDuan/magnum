#ifndef Magnum_Platform_Implementation_OpenGLFunctionLoader_h
#define Magnum_Platform_Implementation_OpenGLFunctionLoader_h
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

#include "Magnum/Magnum.h"

#ifdef CORRADE_TARGET_WINDOWS
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

namespace Magnum { namespace Platform { namespace Implementation {

class OpenGLFunctionLoader {
    public:
        #ifndef CORRADE_GCC46_COMPATIBILITY
        #ifndef CORRADE_TARGET_WINDOWS
        using FunctionPointer = void(*)();
        #else
        using FunctionPointer = PROC;
        #endif
        #else
        typedef void(*FunctionPointer)();
        #endif

        explicit OpenGLFunctionLoader();
        ~OpenGLFunctionLoader();

        FunctionPointer load(const char* name);

    private:
        /* CGL-specific handles */
        #ifdef CORRADE_TARGET_APPLE
        void* library;

        /* WGL-specific handles */
        #elif defined(CORRADE_TARGET_WINDOWS)
        HMODULE library;

        /* GLX-specific handles (nothing needed) */
        #elif defined(CORRADE_TARGET_UNIX) && defined(MAGNUM_PLATFORM_USE_GLX)

        /* EGL-specific handles (nothing needed) */
        #elif defined(MAGNUM_PLATFORM_USE_EGL)

        /* Otherwise unsupported */
        #else
        #error unsupported platform
        #endif
};

}}}

#endif
