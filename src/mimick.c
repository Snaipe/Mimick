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

struct mmk_mock_extended {
    const char *name;
    struct mmk_offset *offsets;
    struct mmk_item *params;
    struct mmk_item *cur_params;
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

struct mmk_item *mmk_pop_params(mmk_mock mock)
{
    struct mmk_mock_extended *mock_ex = (void*) mock;
    struct mmk_item *cur = mock_ex->cur_params;
    if (cur != NULL)
        mock_ex->cur_params = cur->next;
    return cur;
}

void mmk_bind (mmk_mock mock, const char **params_str, void *params) {
    struct mmk_mock_extended *mock_ex = (void*) mock;
    for (const char **ps = params_str; *ps; ++ps) {
        char *end = strchr (*ps + 1, ' ');
        size_t len = (size_t) (end - *ps - 1);
        struct mmk_offset *off = mmk_offsetof (mock_ex->offsets, *ps + 1, len);
        if (off == NULL)
            continue;
        mmk_field_ (bool, params, off->present_offset) = true;
    }
    struct mmk_item *it = &mmk_field_ (struct mmk_item, params, 0);
    struct mmk_item *prev = NULL;
    for (struct mmk_item *i = mock_ex->params; i; prev = i, i = i->next)
        continue;
    if (prev == NULL) {
        mock_ex->params = it;
        mock_ex->cur_params = it;
    } else {
        prev->next = it;
    }
}

mmk_mock mmk_mock_create (const char *name, const char *path, struct mmk_offset *offsets, mmk_fn fn)
{
    plt_lib self = plt_get_lib(plt, path);
    assert(self != NULL);

    plt_fn **off = plt_get_offset(self, name);
    assert (off != NULL);

    struct mmk_mock_extended *ctx = malloc (sizeof (struct mmk_mock_extended));
    *ctx = (struct mmk_mock_extended) {
        .name = name,
        .offsets = offsets,
        .orig = *off,
        .offset = off,
    };
    ctx->trampoline = create_trampoline(ctx, (plt_fn *) fn);
    plt_set_offset(off, ctx->trampoline);

    return (mmk_mock) ctx;
}

void mmk_mock_destroy (mmk_mock mock)
{
    struct mmk_mock_extended *mock_ex = (void*) mock;
    plt_set_offset(mock_ex->offset, mock_ex->orig);
    destroy_trampoline(mock_ex->trampoline);
    free(mock);
}
