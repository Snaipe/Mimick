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
#include <stdlib.h>
#include <assert.h>
#include "mimick.h"
#include "plt.h"
#include "trampoline.h"

struct mmk_mock {
    plt_fn *orig;
    plt_fn **offset;
    plt_fn *trampoline;
};

struct mmk_mock *mmk_ctx;
static plt_ctx plt;

void mmk_init (void)
{
    plt = plt_init_ctx();
    assert(plt != (void*) -1);
}

mmk_mock mmk_mock_create (const char *name, const char *path, mmk_fn fn)
{
    plt_lib self = plt_get_lib(plt, path);
    assert(self != NULL);

    plt_fn **off = plt_get_offset(self, name);
    assert (off != NULL);

    mmk_mock ctx = malloc (sizeof (struct mmk_mock));
    *ctx = (struct mmk_mock) {
        .orig = *off,
        .offset = off,
    };
    ctx->trampoline = create_trampoline(ctx, (plt_fn *) fn);
    *off = ctx->trampoline;

    return ctx;
}

void mmk_mock_destroy (mmk_mock mock)
{
    *mock->offset = mock->orig;
    destroy_trampoline(mock->trampoline);
    free(mock);
}
