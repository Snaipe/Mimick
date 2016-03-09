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

# if __STDC_VERSION__ >= 201112L && !defined __STDC_NO_THREADS__
#  define MMK_THREAD_LOCAL _Thread_Local
# elif defined _MSC_VER
#  define MMK_THREAD_LOCAL __declspec(thread)
# elif defined __GNUC__
#  define MMK_THREAD_LOCAL __thread
# endif

MMK_THREAD_LOCAL struct {
    struct mmk_matcher *matcher;
    struct mmk_params *params;
    const char **order;
    char **params_str;
    int verify;
} matcher_ctx;

static char *mmk_matcher_skipspace(char *buf)
{
    for (; mmk_isspace(*buf); ++buf)
        continue;
    return buf;
}

static char *mmk_matcher_findspace(char *buf)
{
    for (; !mmk_isspace(*buf); ++buf)
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
                case '(': ++paren_lvl;
                case ')': --paren_lvl;
                case '{': ++brace_lvl;
                case '}': --brace_lvl;
                case '[': ++arr_lvl;
                case ']': --arr_lvl;
            }
        }
        if (!paren_lvl && !brace_lvl && !arr_lvl && *buf == ',')
            return buf;
    }
    return NULL;
}


void mmk_matcher_init(int counter, struct mmk_matcher *ctx, char *callexpr)
{
    ctx->prio = counter;
    ctx->next = NULL;

    /* Mark which parameters are special. We do this by parsing
     * the call expression, which should look like this:
     *
     *   call(foo, bar, mmk_any(baz_t))
     *
     * Special parameters are any parameters starting with mmk_*.
     */

    unsigned int markmask = 0;

    char *start = mmk_strchr(callexpr, '(') + 1;
    for (; start != NULL; start = mmk_matcher_next_param(start)) {
        markmask <<= 1;
        start = mmk_matcher_skipspace(start);
        if (mmk_strneq(start, "mmk_", 4))
            markmask |= 1;
    }

    ctx->kind = (enum mmk_matcher_kind) markmask;
    matcher_ctx.matcher = ctx;
    matcher_ctx.params = NULL;
    matcher_ctx.params_str = NULL;
    matcher_ctx.order = NULL;
    matcher_ctx.verify = 0;
}

int mmk_matcher_get_offset(const char **order, char *start, char *end)
{
    uintptr_t len = (uintptr_t) (end - start + 1);
    for (int idx = 0; *order; ++order, ++idx) {
        if (mmk_strneq(*order, start, (size_t) len))
            return idx;
    }
    return 0;
}

void mmk_matcher_init_verify(struct mmk_matcher *ctx, const char **order, char **params)
{
    ctx->next = NULL;

    /* Mark which parameters are special. We do this by parsing
     * the designated initializer parameters, which should look like this:
     *
     *   .that_<name> = value
     *
     * Special parameters are any parameters with a value starting with mmk_*.
     */

    unsigned int markmask = 0;

    matcher_ctx.params_str = params;
    matcher_ctx.order = order;
    matcher_ctx.verify = 1;

    for (; *params; ++params) {
        char *start = mmk_strchr(*params, '.') + 1;
        start = mmk_matcher_skipspace(start);

        if (!mmk_strneq(start, "that_", 5) && !mmk_strneq(start, "times", 5))
            continue;

        char *end = mmk_matcher_findspace(start);
        int submask = 1 << mmk_matcher_get_offset(order, start, end);
        start = mmk_matcher_skipspace(end);
        mmk_assert (*start == '=');
        start = mmk_matcher_skipspace(start + 1);

        if (mmk_strneq(start, "(mmk_", 5))
            markmask |= submask;
    }

#ifdef _MSC_VER
    if (matcher_ctx.matcher)
        ctx->next = matcher_ctx.matcher->next;
#endif

    ctx->kind = (enum mmk_matcher_kind) markmask;
    matcher_ctx.matcher = ctx;
}

void mmk_matcher_reorder_verify(void)
{
    size_t count = 0;
    for (struct mmk_matcher *m = matcher_ctx.matcher->next; m; m = m->next) {

        char *start = mmk_strchr(matcher_ctx.params_str[count], '.') + 1;
        start = mmk_matcher_skipspace(start);
        char *end = mmk_matcher_findspace(start);
        m->prio = mmk_matcher_get_offset(matcher_ctx.order, start, end) + 1;

        ++count;
    }

    /* Insert sort the matcher list */
    struct mmk_matcher *prev = matcher_ctx.matcher;
    for (struct mmk_matcher *m = matcher_ctx.matcher->next; m; m = m->next) {
        for (struct mmk_matcher *m2 = matcher_ctx.matcher; m2; ) {
            struct mmk_matcher *next = m2->next;
            if (!next || next->prio > m->prio) {
                prev->next = m->next;
                m->next = next;
                m2->next = m;
                break;
            }
            m2 = next;
        }
    }
}

void mmk_matcher_term(void)
{
    matcher_ctx.matcher = NULL;
    matcher_ctx.params = NULL;
}

struct mmk_matcher *mmk_matcher_ctx(void)
{
    return matcher_ctx.matcher;
}

struct mmk_params *mmk_matcher_params(void)
{
    return matcher_ctx.params;
}

void mmk_matcher_set_params(struct mmk_params *params)
{
    matcher_ctx.params = params;
}

void mmk_matcher_add(enum mmk_matcher_kind kind, int counter, struct mmk_matcher *out)
{
    struct mmk_matcher *prev = matcher_ctx.matcher;
    size_t prio = counter;

    *out = (struct mmk_matcher) {
        .kind = kind,
        .prio = prio,
    };

    /* This is necessary because MSVC fucks up sequence points with
       compound literals */
#ifdef _MSC_VER
    if (!matcher_ctx.matcher) {
        matcher_ctx.matcher = out;
        return;
    }
#endif

    for (struct mmk_matcher *m = matcher_ctx.matcher;
            m != NULL && m->prio < prio;
            prev = m, m = m->next)
        continue;
    prev->next = out;
}

void (*mmk_matcher_get_predicate(struct mmk_matcher *m))(void)
{
    return (void (*)(void)) (m + 1);
}
