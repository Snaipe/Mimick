/*
 * The MIT License (MIT)
 *
 * Copyright © 2016 Franklin "Snaipe" Mathieu <http://snai.pe/>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef THREADLOCAL_H_
# define THREADLOCAL_H_

# if __STDC_VERSION__ >= 201112L && !defined __STDC_NO_THREADS__
#  define MMK_THREAD_LOCAL(Type) _Thread_Local Type
# elif defined _MSC_VER
#  define MMK_THREAD_LOCAL(Type) __declspec(thread) Type
# elif defined __GNUC__ && defined _WIN32
#  define MMK_THREAD_LOCAL(Type) DWORD
# elif defined __GNUC__
#  define MMK_THREAD_LOCAL(Type) __thread Type
# endif

# if defined __GNUC__ && defined _WIN32
#  include <windows.h>
#  include "mimick/assert.h"

#  define tls_set(Type, Var, Val) __extension__ ({ \
            __typeof__(Type) *v__ = NULL; \
            if ((Var) == 0) { \
                (Var) = TlsAlloc(); \
                v__ = mmk_malloc(sizeof (*v__)); \
                TlsSetValue((Var), v__); \
            } else { \
                v__ = TlsGetValue(Var); \
            } \
            *v__ = (Val); \
        })
#  define tls_get(Type, Var) __extension__ ({ \
            __typeof__(Type)* v__ = TlsGetValue(Var); \
            mmk_assert(v__ != NULL); \
            *v__; \
        })

#  define tls_op(Type, Var, Op, Val) __extension__ ({ \
            __typeof__(Type) *v__ = NULL; \
            if ((Var) == 0) { \
                (Var) = TlsAlloc(); \
                v__ = mmk_malloc(sizeof (*v__)); \
                TlsSetValue((Var), v__); \
                *v__ = (__typeof__(Type)) { 0 }; \
            } else { \
                v__ = TlsGetValue(Var); \
            } \
            *v__ = *v__ Op (Val); \
        })
# else
#  define tls_set(Type, Var, Val) ((Var) = (Val))
#  define tls_get(Type, Var) (Var)
#  define tls_op(Type, Var, Op, Val) ((Var) = (Var) Op (Val))
# endif

#endif /* !THREADLOCAL_H_ */
