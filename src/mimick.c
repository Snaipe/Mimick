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
#include "assert.h"
#include "mimick.h"
#include "plt.h"
#include "trampoline.h"
#include "vitals.h"
#include "mock.h"
#include "threadlocal.h"

static struct {
    plt_ctx plt;
} self;

void mmk_init (void)
{
    self.plt = plt_init_ctx();
    mmk_assert (self.plt != (void*) -1);

    mmk_init_vital_functions (self.plt);
}

void *mmk_stub_context (mmk_stub stub)
{
    return stub->ctx;
}

struct mmk_params *mmk_mock_get_params(void)
{
    mmk_mock mock = mmk_stub_context(mmk_ctx ());
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
    mmk_assert (lib != NULL);

    plt_fn **off = plt_get_offset(lib, name);
    mmk_assert (off != NULL);

    *stub = (struct mmk_stub) {
        .ctx_asked = mmk_ctx_asked,
        .ctx_set = mmk_set_ctx,
        .ctx_get = mmk_ctx,
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

static MMK_THREAD_LOCAL int ask_ctx;

mmk_fn mmk_mock_create_internal (const char *target, mmk_fn fn)
{
    ask_ctx = 0;

    mmk_mock ctx = mmk_malloc (sizeof (struct mmk_mock));
    mmk_assert (ctx);
    *ctx = (struct mmk_mock) {
        .params = NULL,
    };

    char *name_end = strchr(target, '@');
    size_t name_sz;
    if (name_end == NULL)
        name_sz = strlen(target);
    else
        name_sz = (size_t) (name_end - target);
    char *name = mmk_malloc (name_sz + 1);
    strncpy(name, target, name_sz);
    name[name_sz] = '\0';

    ctx->stubs = mmk_stub_create (name, fn, ctx);
    ctx->stubs->next = mmk_stub_create (target, fn, ctx);
    mmk_free(name);
    return (mmk_fn) ctx->stubs->trampoline;
}

struct mmk_stub *mmk_ask_ctx (mmk_fn fn)
{
    ask_ctx = 1;
    return ((struct mmk_stub *(*)(void)) fn)();
}

int mmk_ctx_asked (void)
{
    int asked = ask_ctx;
    ask_ctx = 0;
    return asked;
}

void mmk_mock_destroy_internal (mmk_fn fn)
{
    struct mmk_stub *stub = mmk_ask_ctx (fn);
    struct mmk_mock *mock = mmk_stub_context (stub);

    for (struct mmk_stub *s = mock->stubs; s;) {
        struct mmk_stub *next = s->next;
        mmk_stub_destroy (s);
        s = next;
    }
    mmk_free (mock->call_data);
    for (struct mmk_params *p = mock->params, *pnext; p;) {
        pnext = p->next;
        for (struct mmk_matcher *m = p->matcher_ctx, *next; m;) {
            next = m->next;
            mmk_free (m);
            m = next;
        }
        mmk_free (p);
        p = pnext;
    }
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
    if (!mock->call_data || !mock->call_data_top)
        return NULL;

    return mock->call_data + sizeof (size_t);
}

void *mmk_mock_params_next(mmk_mock mock, void *prev) {
    char *ptr = prev;
    size_t sz = *(size_t*) (ptr - sizeof (size_t));
    ptr += sz;
    if (ptr >= mock->call_data + mock->call_data_top)
        return NULL;
    return ptr;
}

void mmk_verify_register_call (void *params, size_t size)
{
    mmk_mock mock = mmk_stub_context (mmk_ctx ());
    if (!mock->call_data) {
        mock->call_data = mmk_malloc (4096);
        mmk_assert (mock->call_data);
        mock->call_data_size = 4096;
    }

    if (find_and_inc_call_matching(mock, params, size))
        return;

    if (mock->call_data_top + size + sizeof (size_t) >= mock->call_data_size) {
        while (mock->call_data_top + size + sizeof (size_t) >= mock->call_data_size) {
            mock->call_data_size += 4096;
        }
        mock->call_data = mmk_realloc (mock->call_data, mock->call_data_size);
        mmk_assert (mock->call_data);
    }

    mmk_memcpy(mock->call_data + mock->call_data_top, &size, sizeof (size_t));
    size_t *times = mmk_memcpy(mock->call_data + mock->call_data_top
            + sizeof (size_t), params, size);
    *times = 1;

    mock->call_data_top += size + sizeof (size_t);
}

static MMK_THREAD_LOCAL mmk_stub mmk_ctx_;

mmk_stub mmk_ctx(void)
{
    return mmk_ctx_;
}

void mmk_set_ctx(mmk_stub stub)
{
    mmk_ctx_ = stub;
}
