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
#ifndef MIMICK_H_
# define MIMICK_H_

# include "mimick/preprocess.h"

typedef void (*mmk_fn)(void);
typedef struct mmk_stub *mmk_stub;

/* Stub API */

mmk_stub mmk_ctx(void);
void *mmk_stub_context (mmk_stub stub);
mmk_stub mmk_stub_create (const char *target, mmk_fn fn, void *ctx);
void mmk_stub_destroy (mmk_stub stub);

/* Mock API */

# define mmk_val(Type, ...) (&(Type) { __VA_ARGS__ })

# define mmk_mock(Target, Id) <internal>
# define mmk_mock_define(Id, ReturnType, ...) <internal>
# define mmk_mock_define_void(Id, ...) <internal>
# define mmk_when(CallExpr, ...) <internal>
# define mmk_verify(CallExpr, ...) <internal>
# define mmk_reset(Mock) <internal>

/* Matcher API */

# define mmk_any(Type) <internal>
# define mmk_neq(Type, Val) <internal>
# define mmk_lt(Type, Val) <internal>
# define mmk_leq(Type, Val) <internal>
# define mmk_gt(Type, Val) <internal>
# define mmk_geq(Type, Val) <internal>
# define mmk_that(Predicate) <internal>

/* Internals */

# include "mimick/mock.h"
# include "mimick/matcher.h"

# ifndef MMK_DO_NOT_UNOPTIMIZE
#  if defined __GNUC__
#   pragma GCC optimize "O0"
#  elif defined _MSC_VER
#   pragma optimize("", off)
#  endif
# endif

# ifndef MMK_DO_NOT_DISABLE_WARNINGS
#  if defined __GNUC__
#   pragma GCC diagnostic ignored "-Wunused-result"
#  elif defined _MSC_VER
#   pragma warning(disable : 6031)
#  endif
# endif

#endif /* !MIMICK_H_ */
