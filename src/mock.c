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

#include "assert.h"
#include "core.h"
#include "mimick.h"
#include "mock.h"
#include "vitals.h"

mmk_fn mmk_mock_create_internal (const char *target, mmk_fn fn)
{
    mmk_init();

    struct mmk_mock_ctx *ctx = mmk_malloc (sizeof (struct mmk_mock_ctx));
    mmk_assert (ctx);
    *ctx = (struct mmk_mock_ctx) {
        .params = NULL,
    };

    char *name_end = mmk_strchr(target, '@');
    size_t name_sz;
    if (name_end == NULL)
        name_sz = mmk_strlen(target);
    else
        name_sz = (size_t) (name_end - target);
    char *name = mmk_malloc (name_sz + 1);
    mmk_strncpy(name, target, name_sz);
    name[name_sz] = '\0';

    ctx->stubs = mmk_stub_create (name, fn, ctx);
    if (name_end && !mmk_strneq(name_end + 1, "self", 4))
        ctx->stubs->next = mmk_stub_create (target, fn, ctx);
    mmk_free(name);
    return (mmk_fn) ctx->stubs->trampoline;
}

void mmk_mock_destroy_internal (struct mmk_mock_ctx *mock)
{
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

void *mmk_mock_params_begin(struct mmk_mock_ctx *mock) {
    if (!mock->call_data || !mock->call_data_top)
        return NULL;

    return mock->call_data + sizeof (size_t);
}

void *mmk_mock_params_next(struct mmk_mock_ctx *mock, void *prev) {
    char *ptr = prev;
    size_t sz = *(size_t*) (ptr - sizeof (size_t));
    ptr += sz;
    if (ptr >= mock->call_data + mock->call_data_top)
        return NULL;
    return ptr;
}

struct mmk_params *mmk_mock_get_params(void)
{
    struct mmk_mock_ctx *mock = mmk_stub_context(mmk_ctx ());
    return mock->params;
}
