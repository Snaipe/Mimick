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
#ifndef MIMICK_MOCK_H_
# define MIMICK_MOCK_H_

# include <stdlib.h>
# include <stdio.h>
# include "matcher.h"
# include "alloc.h"
# include "when.h"
# include "verify.h"

struct mmk_params {
    struct mmk_matcher *matcher_ctx;
    struct mmk_params *next;
};

struct mmk_params *mmk_mock_get_params(void);
void *mmk_mock_params_begin(mmk_mock mock);
void *mmk_mock_params_next(mmk_mock mock, void *prev);

# undef mmk_mock_create
# define mmk_mock_create(Target, Id) (MMK_MANGLE(Id, fn)) mmk_mock_create_internal((Target), (mmk_fn) MMK_MANGLE(Id, stub))

# define MMK_MK_ARG_STR(_, X) #X,

# define MMK_MANGLE_(Id, Name) mmkuser_ ## Id ## _ ## Name
# define MMK_MANGLE(Id, Name) MMK_MANGLE_(Id, Name)

# define MMK_DEF_VERIFY_PARAM_(X) .X = X,
# define MMK_DEF_VERIFY_PARAM(_, T, X) MMK_DEF_VERIFY_PARAM_(X)

# define MMK_DEF_FIELD_(T, X) T X;
# define MMK_DEF_FIELD(_, T, X) MMK_DEF_FIELD_(T, X)

# define MMK_TRYMATCH(Name, Type, Param) \
    if (markmask & 1) { \
        m = m->next; \
        if (m->kind >= MMK_MATCHER_CMP_START \
                && m->kind <= MMK_MATCHER_CMP_END) { \
            int res = mmk_memcmp(&Param, m + 1, sizeof (Type)); \
            if (res < 0) \
                if (m->kind != MMK_MATCHER_NEQ \
                        && m->kind != MMK_MATCHER_LT) \
                    continue; \
            if (res > 0) \
                if (m->kind != MMK_MATCHER_NEQ \
                        && m->kind != MMK_MATCHER_GT) \
                    continue; \
            if (m->kind != MMK_MATCHER_LEQ \
                    && m->kind != MMK_MATCHER_GEQ) \
                continue; \
        } else if (m->kind == MMK_MATCHER_THAT) { \
            int (*predicate)(Type, Type) = (int (*)(Type, Type)) \
                    mmk_matcher_get_predicate(m); \
            if (!predicate(Param, *(Type*)(m + 1))) \
                continue; \
        } \
    } else { \
        if (mmk_memcmp(&Param, &bind->params.Param, sizeof (Type))) \
            continue; \
    } \
    markmask >>= 1;

# define MMK_TRYVERIFY(Id, Type, Param) \
    if (markmask & 1) { \
        m = m->next; \
        if (m->kind >= MMK_MATCHER_CMP_START \
                && m->kind <= MMK_MATCHER_CMP_END) { \
            int res = mmk_memcmp(&p->Param, \
                    &Param, sizeof (Type)); \
            if (res < 0 \
                    && m->kind != MMK_MATCHER_NEQ \
                    && m->kind != MMK_MATCHER_LT) \
                goto fail; \
            else if (res > 0 \
                    && m->kind != MMK_MATCHER_NEQ \
                    && m->kind != MMK_MATCHER_GT) \
                goto fail; \
            else if (res == 0 \
                    && m->kind != MMK_MATCHER_LEQ \
                    && m->kind != MMK_MATCHER_GEQ) \
                goto fail; \
        } else if (m->kind == MMK_MATCHER_THAT) { \
            int (*predicate)(Type, Type) = (int (*)(Type, Type)) \
                    mmk_matcher_get_predicate(m); \
            if (!predicate(p->Param, Param)) \
                goto fail; \
        } \
    } else { \
        if (p->Param != Param) \
            goto fail; \
    } \
    markmask >>= 1;

# define MMK_SET_PARAMS(Id, T, X) bind->params.X = X;

# undef mmk_mock_define
# define mmk_mock_define(Id, ...)                                              \
    typedef MMK_EXPAND(MMK_VA_HEAD(__VA_ARGS__)) MMK_MANGLE(Id, returntype);   \
    typedef MMK_MANGLE(Id, returntype)(*MMK_MANGLE(Id, fn))(                   \
            MMK_EXPAND(MMK_PARAM_LIST(MMK_VA_TAIL(__VA_ARGS__))));             \
    struct MMK_MANGLE(Id, params) {                                            \
        size_t mmk_times__;  \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_FIELD, Id, MMK_VA_TAIL(__VA_ARGS__)))\
    };                                                                         \
    struct MMK_MANGLE(Id, binding) {                                           \
        struct mmk_params item;                                              \
        struct mmk_result result;                                              \
        struct MMK_MANGLE(Id, params) params; \
    };                                                                         \
    static MMK_MANGLE(Id, returntype) MMK_MANGLE(Id, stub)(                    \
        MMK_EXPAND(MMK_PARAM_LIST(MMK_VA_TAIL(__VA_ARGS__))))                  \
    {                                                                          \
        static MMK_MANGLE(Id, returntype) zero__; \
        struct mmk_matcher *matcher_ctx = mmk_matcher_ctx();                   \
        if (matcher_ctx) { \
            struct mmk_mock *mock = mmk_stub_context(mmk_ctx); \
            if (matcher_ctx->kind == 0) { \
                struct MMK_MANGLE(Id, binding) *bind = mmk_malloc (sizeof (struct MMK_MANGLE(Id, binding))); \
                bind->result = *mmk_when_get_result(); \
                MMK_PAIR_APPLY(MMK_SET_PARAMS, Id, MMK_VA_TAIL(__VA_ARGS__))            \
                mmk_when_impl(mock, bind); \
            } else if (matcher_ctx->kind == 1) { \
                size_t times = 0;                                              \
                for (struct MMK_MANGLE(Id, params) *p =                 \
                            mmk_mock_params_begin(mock);                       \
                        p; p = mmk_mock_params_next(mock, p))                  \
                {                                                              \
                    struct mmk_matcher *m = matcher_ctx;                       \
                    unsigned int markmask = (unsigned int) m->kind;            \
                    MMK_EXPAND(MMK_PAIR_APPLY(MMK_TRYVERIFY, Id,               \
                            MMK_VA_TAIL(__VA_ARGS__)))                         \
                    times += p->mmk_times__;                                    \
            fail:                                                              \
                    continue;                                                  \
                }                                                              \
                mmk_verify_set_times(times); \
            } \
            return zero__; \
        } \
        mmk_verify_register_call (&(struct MMK_MANGLE(Id, params)) {    \
                MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_VERIFY_PARAM, Id,            \
                            MMK_VA_TAIL(__VA_ARGS__)))                         \
            }, sizeof (struct MMK_MANGLE(Id, params)));                 \
        struct mmk_params *param = mmk_mock_get_params();                      \
        for (; param; param = param->next) {                                   \
            struct MMK_MANGLE(Id, binding) *bind = (void*) param;                  \
            struct mmk_matcher *m = param->matcher_ctx;                        \
            size_t markmask = m->prio;                    \
            MMK_EXPAND(MMK_PAIR_APPLY(MMK_TRYMATCH, Id,                        \
                    MMK_VA_TAIL(__VA_ARGS__)))                                 \
            if (bind->result.then_errno)                                               \
                errno = bind->result.then_errno;                                       \
            if (bind->result.then_return) \
                return *(MMK_MANGLE(Id, returntype)*) bind->result.then_return;            \
            return zero__; \
        }                                                                      \
        return zero__; \
    }                                                                          \
    typedef int MMK_MANGLE(Id, dummy)

mmk_fn mmk_mock_create_internal (const char *target, mmk_fn fn);
struct mmk_item *mmk_pop_params (void);
void mmk_bind (mmk_mock mock, const char **params_str, void *params);

#endif /* !MIMICK_MOCK_H_ */
