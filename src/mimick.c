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

#undef mmk_mock_create

struct mmk_stub {
    void *ctx;
    plt_fn *orig;
    plt_fn **offset;
    plt_fn *trampoline;
};

struct mmk_mock {
    const char *name;
    struct mmk_offset *offsets;
    struct mmk_item *params;
    struct mmk_item *cur_params;
    struct mmk_stub stub;
};

mmk_stub mmk_ctx;
static plt_ctx plt;

void mmk_init (void)
{
    plt = plt_init_ctx();
    assert(plt != (void*) -1);
}

struct mmk_item *mmk_pop_params (void)
{
    mmk_mock mock = mmk_stub_context (mmk_ctx);
    struct mmk_item *cur = mock->cur_params;
    if (cur != NULL)
        mock->cur_params = cur->next;
    return cur;
}

void mmk_bind (mmk_mock mock, const char **params_str, void *params) {
    for (const char **ps = params_str; *ps; ++ps) {
        char *end = strchr (*ps + 1, ' ');
        size_t len = (size_t) (end - *ps - 1);
        struct mmk_offset *off = mmk_offsetof (mock->offsets, *ps + 1, len);
        if (off == NULL)
            continue;
        mmk_field_ (bool, params, off->present_offset) = true;
    }
    struct mmk_item *it = &mmk_field_ (struct mmk_item, params, 0);
    struct mmk_item *prev = NULL;
    for (struct mmk_item *i = mock->params; i; prev = i, i = i->next)
        continue;
    if (prev == NULL) {
        mock->params = it;
        mock->cur_params = it;
    } else {
        prev->next = it;
    }
}

void *mmk_stub_context (mmk_stub stub)
{
    return stub->ctx;
}

void mmk_stub_create_static (mmk_stub stub, const char *name, const char *path, mmk_fn fn, void *ctx)
{
    plt_lib self = plt_get_lib(plt, path);
    assert(self != NULL);

    plt_fn **off = plt_get_offset(self, name);
    assert (off != NULL);

    *stub = (struct mmk_stub) {
        .ctx = ctx,
        .orig = *off,
        .offset = off,
    };
    stub->trampoline = create_trampoline(stub, (plt_fn *) fn);
    plt_set_offset(off, stub->trampoline);
}

mmk_stub mmk_stub_create (const char *name, const char *path, mmk_fn fn, void *ctx)
{
    mmk_stub stub = malloc (sizeof (struct mmk_stub));
    mmk_stub_create_static (stub, name, path, fn, ctx);
    return stub;
}

void mmk_stub_destroy_static (mmk_stub stub)
{
    plt_set_offset (stub->offset, stub->orig);
    destroy_trampoline (stub->trampoline);
}

void mmk_stub_destroy (mmk_stub stub)
{
    mmk_stub_destroy_static (stub);
    free (stub);
}

mmk_mock mmk_mock_create (const char *name, const char *path, struct mmk_offset *offsets, mmk_fn fn)
{
    mmk_mock ctx = malloc (sizeof (struct mmk_mock));
    *ctx = (struct mmk_mock) {
        .name = name,
        .offsets = offsets,
    };
    mmk_stub_create_static (&ctx->stub, name, path, fn, ctx);

    return ctx;
}

void mmk_mock_destroy (mmk_mock mock)
{
    mmk_stub_destroy_static (&mock->stub);
}
