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
#include "vitals.h"

struct mmk_stub {
    void *ctx;
    char *name;
    char *path;
    plt_fn *orig;
    plt_fn **offset;
    plt_fn *trampoline;
};

struct mmk_mock {
    struct mmk_params *params;
    struct mmk_stub stub;
    char *call_data;
    size_t call_data_top;
    size_t call_data_size;
};

mmk_stub mmk_ctx;
static struct {
    plt_ctx plt;
} self;

void mmk_init (void)
{
    self.plt = plt_init_ctx();
    assert(self.plt != (void*) -1);

    mmk_init_vital_functions (self.plt);
}

void *mmk_stub_context (mmk_stub stub)
{
    return stub->ctx;
}

void mmk_when_impl (mmk_mock mock, struct mmk_params *params)
{
    params->matcher_ctx = mmk_matcher_ctx();
    params->next = mock->params;
    mock->params = params;
    mmk_matcher_term ();
}

struct mmk_params *mmk_mock_get_params(void)
{
    mmk_mock mock = mmk_stub_context(mmk_ctx);
    return mock->params;
}

void mmk_stub_create_static (mmk_stub stub, const char *target, mmk_fn fn, void *ctx)
{
    char *name = mmk_malloc (strlen(target) + 1);
    strcpy(name, target);

    char *path = NULL;
    char *delim = strchr(name, '@');
    if (delim != NULL) {
        *delim = 0;
        path = delim + 1;
    }

    plt_lib lib = plt_get_lib(self.plt, path);
    assert(lib != NULL);

    plt_fn **off = plt_get_offset(lib, name);
    assert (off != NULL);

    *stub = (struct mmk_stub) {
        .ctx = ctx,
        .name = name,
        .path = path,
        .orig = *off,
        .offset = off,
    };
    stub->trampoline = create_trampoline(stub, (plt_fn *) fn);
    plt_set_offset(off, stub->trampoline);
}

mmk_stub mmk_stub_create (const char *target, mmk_fn fn, void *ctx)
{
    mmk_stub stub = mmk_malloc (sizeof (struct mmk_stub));
    mmk_stub_create_static (stub, target, fn, ctx);
    return stub;
}

void mmk_stub_destroy_static (mmk_stub stub)
{
    plt_set_offset (stub->offset, stub->orig);
    destroy_trampoline (stub->trampoline);
    mmk_free (stub->name);
}

void mmk_stub_destroy (mmk_stub stub)
{
    mmk_stub_destroy_static (stub);
    mmk_free (stub);
}

mmk_mock mmk_mock_create_internal (const char *target, mmk_fn fn)
{
    mmk_mock ctx = mmk_malloc (sizeof (struct mmk_mock));
    assert(ctx);
    *ctx = (struct mmk_mock) {
        .params = NULL,
    };
    mmk_stub_create_static (&ctx->stub, target, fn, ctx);
    return ctx;
}

void mmk_mock_destroy (mmk_mock mock)
{
    mmk_stub_destroy_static (&mock->stub);
    mmk_free (mock);
}

static int find_and_inc_call_matching (mmk_mock mock, void *params, size_t size)
{
    // skip .times field
    params = (void*) ((char *) params + sizeof (size_t));
    size -= sizeof (size_t);

    for (void *p = mmk_mock_params_begin(mock); p;
            p = mmk_mock_params_next(mock, p))
    {
        // compare parameters w/o .times
        int res = mmk_memcmp((char *) p + sizeof (size_t), params, size);
        if (!res) {
            size_t *times = p;
            ++*times;
            return 1;
        }
    }
    return 0;
}

void *mmk_mock_params_begin(mmk_mock mock) {
    if (!mock->call_data)
        return NULL;

    return mock->call_data + sizeof (size_t);
}

void *mmk_mock_params_next(mmk_mock mock, void *prev) {
    char *ptr = prev;
    if (ptr >= mock->call_data + mock->call_data_top)
        return NULL;
    size_t sz = *(size_t*) (ptr - sizeof (size_t));
    return ptr + sz;
}

void mmk_verify_register_call (void *params, size_t size)
{
    mmk_mock mock = mmk_stub_context (mmk_ctx);
    if (!mock->call_data) {
        mock->call_data = mmk_malloc (4096);
        assert (mock->call_data);
        mock->call_data_size = 4096;
    }

    if (find_and_inc_call_matching(mock, params, size))
        return;

    if (mock->call_data_top + size + sizeof (size_t) >= mock->call_data_size) {
        while (mock->call_data_top + size + sizeof (size_t) >= mock->call_data_size) {
            mock->call_data_size += 4096;
        }
        mock->call_data = mmk_realloc (mock->call_data, mock->call_data_size);
        assert (mock->call_data);
    }

    mmk_memcpy(mock->call_data + mock->call_data_top, &size, sizeof (size_t));
    size_t *times = mmk_memcpy(mock->call_data + mock->call_data_top
            + sizeof (size_t), params, size);
    *times = 1;

    mock->call_data_top += size + sizeof (size_t);
}
