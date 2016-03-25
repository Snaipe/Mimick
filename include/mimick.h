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

# include <stdint.h>
# include <stdbool.h>
# include <string.h>
# include "mimick/preprocess.h"
# include "mimick/item.h"
# include "mimick/offset.h"
# include "mimick/matcher.h"

void mmk_init (void);

/* Stub API */

typedef void (*mmk_fn)(void);
typedef struct mmk_stub *mmk_stub;

mmk_stub mmk_ctx(void);

void *mmk_stub_context (mmk_stub stub);
mmk_stub mmk_stub_create (const char *target, mmk_fn fn, void *ctx);
void mmk_stub_destroy (mmk_stub stub);

/* Mock API */

struct mmk_mock_ctx;

# define mmk_val(Type, ...) (&(Type) { __VA_ARGS__ })

# define mmk_mock(Target, Id) <internal>
# define mmk_mock_define(Id, ReturnType, ...) <internal>
# define mmk_mock_define_void(Id, ReturnType, ...) <internal>
# define mmk_when(CallExpr, ...) <internal>
# define mmk_verify(CallExpr, ...) <internal>
# define mmk_reset(Mock) <internal>

# include "mimick/mock.h"

#endif /* !MIMICK_H_ */
