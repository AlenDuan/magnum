#ifndef Magnum_Implementation_TransformFeedbackState_h
#define Magnum_Implementation_TransformFeedbackState_h
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

#include "Magnum/Context.h"

#ifdef MAGNUM_TARGET_GLES2
#error this header is not available in OpenGL ES 2.0 build
#endif

/* If not included, the following members have bad offsets, causing weird
   runtime behavior */
#ifdef CORRADE_MSVC2013_COMPATIBILITY
#include "Magnum/TransformFeedback.h"
#endif

namespace Magnum { namespace Implementation {

struct TransformFeedbackState {
    explicit TransformFeedbackState(Context& context, std::vector<std::string>& extensions);

    void reset();

    GLint maxInterleavedComponents,
        maxSeparateAttributes,
        maxSeparateComponents;
    #ifndef MAGNUM_TARGET_GLES
    GLint maxBuffers;
    #endif

    GLuint binding;

    void(TransformFeedback::*createImplementation)();
    void(TransformFeedback::*attachRangeImplementation)(GLuint, Buffer&, GLintptr, GLsizeiptr);
    void(TransformFeedback::*attachBaseImplementation)(GLuint, Buffer&);
    void(TransformFeedback::*attachRangesImplementation)(GLuint, std::initializer_list<std::tuple<Buffer*, GLintptr, GLsizeiptr>>);
    void(TransformFeedback::*attachBasesImplementation)(GLuint, std::initializer_list<Buffer*>);
};

}}

#endif
