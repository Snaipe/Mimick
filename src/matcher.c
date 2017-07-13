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
#include <stddef.h>
#include <stdint.h>

#include "mimick/assert.h"
#include "mimick/matcher.h"

#include "threadlocal.h"
#include "vitals.h"

static MMK_THREAD_LOCAL(struct mmk_matcher *) matcher_ctx;

void mmk_matcher_init(int kind)
{
    struct mmk_matcher *ctx = mmk_malloc(sizeof (struct mmk_matcher));
    *ctx = (struct mmk_matcher) {
        .kind = (enum mmk_matcher_kind) kind,
    };
    tls_set(struct mmk_matcher *, matcher_ctx, ctx);
}

void mmk_matcher_term(void)
{
    tls_set(struct mmk_matcher *, matcher_ctx, NULL);
}

struct mmk_matcher *mmk_matcher_ctx(void)
{
    return tls_get(struct mmk_matcher *, matcher_ctx);
}

void mmk_matcher_add_data(enum mmk_matcher_kind kind, int counter, void *data)
{
    struct mmk_matcher *out = mmk_malloc(sizeof (struct mmk_matcher));
    struct mmk_matcher *prev = tls_get(struct mmk_matcher *, matcher_ctx);

    *out = (struct mmk_matcher) {
        .kind = kind,
        .prio = counter,
        .data = data,
    };

    for (struct mmk_matcher *m = tls_get(struct mmk_matcher *, matcher_ctx)->next;
            m != NULL && m->prio < out->prio;
            prev = m, m = m->next)
        continue;
    out->next = prev->next;
    prev->next = out;
}

void mmk_matcher_add_fn(enum mmk_matcher_kind kind, int counter, void (*fn)(void))
{
    mmk_matcher_add_data(kind, counter, (void *)fn);
}

void mmk_matcher_add(enum mmk_matcher_kind kind, int counter)
{
    mmk_matcher_add_data(kind, counter, NULL);
}

void (*mmk_matcher_get_predicate(struct mmk_matcher *m))(void)
{
    return (void (*)(void)) m->data;
}
