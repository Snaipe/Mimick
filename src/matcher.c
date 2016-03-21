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
#include "assert.h"
#include "mimick/matcher.h"
#include "vitals.h"
#include "threadlocal.h"

MMK_THREAD_LOCAL struct {
    struct mmk_matcher *matcher;
} matcher_ctx;

static char *mmk_matcher_skipspace(char *buf)
{
    for (; mmk_isspace(*buf); ++buf)
        continue;
    return buf;
}

static char *mmk_matcher_next_param(char *buf)
{
    int in_chr = 0;
    int in_str = 0;
    int paren_lvl = 0;
    int brace_lvl = 0;
    int arr_lvl = 0;
    for (; *buf && paren_lvl >= 0; ++buf) {
        if (!in_chr && *buf == '"' && buf[-1] != '\\')
            in_str ^= 1;
        if (!in_str && *buf == '\'' && buf[-1] != '\\')
            in_chr ^= 1;
        if (!in_str && !in_chr) {
            switch (*buf) {
                case '(': ++paren_lvl; break;
                case ')': --paren_lvl; break;
                case '{': ++brace_lvl; break;
                case '}': --brace_lvl; break;
                case '[': ++arr_lvl;   break;
                case ']': --arr_lvl;   break;
            }
        }
        if (!paren_lvl && !brace_lvl && !arr_lvl && *buf == ',')
            return buf + 1;
    }
    return NULL;
}

void mmk_matcher_init(int kind, char *callexpr)
{
    struct mmk_matcher *ctx = mmk_malloc (sizeof (struct mmk_matcher));
    ctx->kind = (enum mmk_matcher_kind) kind;
    ctx->next = NULL;

    /* Mark which parameters are special. We do this by parsing
     * the call expression, which should look like this:
     *
     *   call(foo, bar, mmk_any(baz_t))
     *
     * Special parameters are any parameters starting with mmk_*.
     */

    size_t markmask = 0;

    char *start = mmk_strchr(callexpr, '(') + 1;
    for (; start != NULL; start = mmk_matcher_next_param(start)) {
        markmask <<= 1;
        start = mmk_matcher_skipspace(start);
        if (mmk_strneq(start, "mmk_", 4))
            markmask |= 1;
    }

    ctx->prio = markmask;
    matcher_ctx.matcher = ctx;
}

void mmk_matcher_term(void)
{
    matcher_ctx.matcher = NULL;
}

struct mmk_matcher *mmk_matcher_ctx(void)
{
    return matcher_ctx.matcher;
}

void mmk_matcher_add(enum mmk_matcher_kind kind, int counter)
{
    struct mmk_matcher *out = mmk_malloc (sizeof (struct mmk_matcher));
    struct mmk_matcher *prev = matcher_ctx.matcher;

    *out = (struct mmk_matcher) {
        .kind = kind,
        .prio = counter,
    };

    for (struct mmk_matcher *m = matcher_ctx.matcher->next;
            m != NULL && m->prio < out->prio;
            prev = m, m = m->next)
        continue;
    out->next = prev->next;
    prev->next = out;
}

void (*mmk_matcher_get_predicate(struct mmk_matcher *m))(void)
{
    return (void (*)(void)) (m + 1);
}
