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

struct mmk_params {
    struct mmk_matcher *matcher_ctx;
    struct mmk_params *next;
};

struct mmk_params *mmk_mock_get_params(void);
void mmk_verify_register_call (void *params, size_t size);
void *mmk_mock_params_begin(mmk_mock mock);
void *mmk_mock_params_next(mmk_mock mock, void *prev);

# undef mmk_mock_create
# define mmk_mock_create(Target, Id) mmk_mock_create_internal((Target), (mmk_fn) MMK_MANGLE(Id, stub))

/* We use the macro naming convention so it can actually be overriden by a
   macro. */
static inline void MMK_VERIFY_FAILED(const char *id, const char *param_str)
{
    fprintf(stderr, "%s:%d: Assertion failed in mock '%s': there were no "
            "interaction matching '%s'.\n", __FILE__, __LINE__,
            id, param_str);
    abort();
}

# define MMK_MK_ARG_STR(_, X) #X,

# define MMK_MANGLE_(Id, Name) mmkuser_ ## Id ## _ ## Name
# define MMK_MANGLE(Id, Name) MMK_MANGLE_(Id, Name)

# define MMK_DEF_VERIFY_FIELD_(T, X) T that_ ## X;
# define MMK_DEF_VERIFY_FIELD(_, T, X) MMK_DEF_VERIFY_FIELD_(T, X)

# define MMK_DEF_VERIFY_PARAM_(X) .that_ ## X = X,
# define MMK_DEF_VERIFY_PARAM(_, T, X) MMK_DEF_VERIFY_PARAM_(X)

# define MMK_TRYMATCH(Name, Type, Param) do { \
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
            if (!mmk_memcmp(&size, m + 1, sizeof (Type))) \
                continue; \
        } \
        markmask >>= 1; \
    } while (0);

# define MMK_TRYVERIFY(Id, Type, Param) do { \
        if (markmask & 1) { \
            m = m->next; \
            if (m->kind >= MMK_MATCHER_CMP_START \
                    && m->kind <= MMK_MATCHER_CMP_END) { \
                int res = mmk_memcmp(&p->that_ ## Param, \
                        &ref->that_ ## Param, sizeof (Type)); \
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
                if (!predicate(p->that_ ## Param, ref->that_ ## Param)) \
                    goto fail; \
            } \
        } else { \
            if (p->that_ ## Param != ref->that_ ## Param) \
                goto fail; \
        } \
        markmask >>= 1; \
    } while (0);

# define MMK_MARK_BITS(Id, T, X) \
    if (markmask & 1) { \
        m = m->next; \
        *((T *) (m + 1)) = (X); \
    } \
    markmask >>= 1;

# define MMK_DEF_ORDER(Id, T, X) "that_" #X,

# undef mmk_mock_define
# define mmk_mock_define(Id, ...)                                              \
    typedef MMK_EXPAND(MMK_VA_HEAD(__VA_ARGS__)) MMK_MANGLE(Id, returntype);   \
    struct MMK_MANGLE(Id, result_params) {                                     \
        int then_errno;                                                        \
        MMK_MANGLE(Id, returntype) then_return;                                \
    };                                                                         \
    struct MMK_MANGLE(Id, verify_params) {                                     \
        size_t times;                                                          \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_VERIFY_FIELD, Id,                    \
                    MMK_VA_TAIL(__VA_ARGS__)))                                 \
    };                                                                         \
    struct MMK_MANGLE(Id, params) {                                            \
        struct mmk_params params;                                              \
        struct MMK_MANGLE(Id, result_params) result;                           \
    };                                                                         \
    static const char *MMK_MANGLE(Id, verify_order)[] = {                      \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_ORDER, Id, MMK_VA_TAIL(__VA_ARGS__)))\
        NULL                                                                   \
    };                                                                         \
    static void MMK_MANGLE(Id, with)(                                          \
        MMK_EXPAND(MMK_PARAM_LIST(MMK_VA_TAIL(__VA_ARGS__))))                  \
    {                                                                          \
        struct mmk_matcher *m = mmk_matcher_ctx();                             \
        unsigned int markmask = (unsigned int) m->kind;                        \
        MMK_PAIR_APPLY(MMK_MARK_BITS, Id, MMK_VA_TAIL(__VA_ARGS__))            \
    }                                                                          \
    static int MMK_MANGLE(Id, verify)(                                         \
        mmk_mock mock,                                                         \
        struct MMK_MANGLE(Id, verify_params) *ref,                             \
        const char *params)                                                    \
    {                                                                          \
        struct mmk_matcher *matcher_ctx = mmk_matcher_ctx();                   \
        size_t times = ref->times;                                             \
        for (struct MMK_MANGLE(Id, verify_params) *p =                         \
                    mmk_mock_params_begin(mock);                               \
                p; p = mmk_mock_params_next(mock, p))                          \
        {                                                                      \
            struct mmk_matcher *m = matcher_ctx;                               \
            unsigned int markmask = (unsigned int) m->kind;                    \
            MMK_EXPAND(MMK_PAIR_APPLY(MMK_TRYVERIFY, Id,                       \
                    MMK_VA_TAIL(__VA_ARGS__)))                                 \
            times -= p->times;                                                 \
    fail:                                                                      \
            continue;                                                          \
        }                                                                      \
        mmk_matcher_term();                                                    \
        if (times != 0) {                                                      \
            MMK_VERIFY_FAILED(#Id, params);                                    \
        }                                                                      \
        return times == 0;                                                     \
    }                                                                          \
    static MMK_MANGLE(Id, returntype) MMK_MANGLE(Id, stub)(                    \
        MMK_EXPAND(MMK_PARAM_LIST(MMK_VA_TAIL(__VA_ARGS__))))                  \
    {                                                                          \
        mmk_verify_register_call (&(struct MMK_MANGLE(Id, verify_params)) {    \
                MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_VERIFY_PARAM, Id,            \
                            MMK_VA_TAIL(__VA_ARGS__)))                         \
            }, sizeof (struct MMK_MANGLE(Id, verify_params)));                 \
        struct mmk_params *param = mmk_mock_get_params();                      \
        for (; param; param = param->next) {                                   \
            struct MMK_MANGLE(Id, result_params) *res = (void*)(param + 1);    \
            struct mmk_matcher *m = param->matcher_ctx;                        \
            unsigned int markmask = (unsigned int) m->kind;                    \
            MMK_EXPAND(MMK_PAIR_APPLY(MMK_TRYMATCH, Id,                        \
                    MMK_VA_TAIL(__VA_ARGS__)))                                 \
            if (res->then_errno)                                               \
                errno = res->then_errno;                                       \
            return res->then_return;                                           \
        }                                                                      \
        return (MMK_MANGLE(Id, returntype)) { 0 };                             \
    }                                                                          \
    typedef int MMK_MANGLE(Id, dummy)

void mmk_when_impl (mmk_mock mock, struct mmk_params *params);

# undef mmk_when
# define mmk_when(Instance, Id, CallExpr, ...) \
        (mmk_matcher_init(__COUNTER__, &(struct mmk_matcher) { .kind = 0 }, #CallExpr), \
        mmkuser_ ## Id ## _ ## CallExpr, \
        mmk_when_impl((Instance), (struct mmk_params *) &(struct mmkuser_ ## Id ## _params) { .result = { __VA_ARGS__ } }))

# undef mmk_verify
# define mmk_verify(Instance, Id, ...) \
        (mmk_matcher_init_verify(&(struct mmk_matcher) { .kind = 0 }, MMK_MANGLE(Id, verify_order), (char *[]) { MMK_APPLY(MMK_MK_ARG_STR, _, __VA_ARGS__) NULL }), \
        MMK_MANGLE(Id, verify)((Instance), &(struct MMK_MANGLE(Id, verify_params)) { __VA_ARGS__ }, #__VA_ARGS__))

mmk_mock mmk_mock_create_internal (const char *target, mmk_fn fn);
struct mmk_item *mmk_pop_params (void);
void mmk_bind (mmk_mock mock, const char **params_str, void *params);

#endif /* !MIMICK_MOCK_H_ */
