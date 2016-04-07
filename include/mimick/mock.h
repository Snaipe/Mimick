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

typedef struct mmk_mock_ctx *mmk_mock_ctx;

# include <errno.h>

# include "matcher.h"
# include "alloc.h"
# include "when.h"
# include "verify.h"

struct mmk_params {
    struct mmk_matcher *matcher_ctx;
    struct mmk_params *next;
};

struct mmk_params *mmk_mock_get_params(void);
void *mmk_mock_params_begin(struct mmk_mock_ctx *mock);
void *mmk_mock_params_next(struct mmk_mock_ctx *mock, void *prev);
mmk_fn mmk_mock_create_internal(const char *target, mmk_fn fn);

# undef mmk_reset
void mmk_reset(mmk_fn fn);
# define mmk_reset(Fn) mmk_reset((mmk_fn) Fn);

# undef mmk_mock
# define mmk_mock(Target, Id) (MMK_MANGLE(Id, create)((Target)))

# define MMK_MK_ARG_STR(_, X) #X,

# define MMK_MANGLE_(Id, Name) mmkuser_ ## Id ## _ ## Name
# define MMK_MANGLE(Id, Name) MMK_MANGLE_(Id, Name)

# define MMK_DEF_VERIFY_PARAM_(X) .X = X,
# define MMK_DEF_VERIFY_PARAM(N, _, T) MMK_DEF_VERIFY_PARAM_(param ## N)

# define MMK_DEF_FIELD_(N, T) T param ## N;
# define MMK_DEF_FIELD(N, _, T) MMK_DEF_FIELD_(N, T)

# define MMK_TRYMATCH(N, Name, Type)                                           \
    if (m->next) {                                                             \
        m = m->next;                                                           \
        if (m->kind == MMK_MATCHER_ANY) {                                      \
            /* Ugly but keeps the indentation level as it is */                \
        } else if (m->kind & MMK_MATCHER_BIT_CMP) {                            \
            int res = mmk_memcmp(&param ## N, &param ## N, sizeof (Type));     \
            if (res == 0 && !(m->kind & MMK_MATCHER_BIT_EQ))                   \
                continue;                                                      \
            if (res < 0 && !(m->kind & MMK_MATCHER_BIT_LT))                    \
                continue;                                                      \
            if (res > 0 && !(m->kind & MMK_MATCHER_BIT_GT))                    \
                continue;                                                      \
        } else if (m->kind == MMK_MATCHER_THAT) {                              \
            int (*predicate)(Type) = (int (*)(Type))                           \
                    mmk_matcher_get_predicate(m);                              \
            if (!predicate(param ## N))                                        \
                continue;                                                      \
        }                                                                      \
    } else {                                                                   \
        if (mmk_memcmp(&param ## N, &bind->params.param ## N, sizeof (Type)))  \
            continue;                                                          \
    }

# define MMK_TRYVERIFY(N, Id, Type)                                            \
    if (m->next) {                                                             \
        m = m->next;                                                           \
        if (m->kind == MMK_MATCHER_ANY) {                                      \
            /* Ugly but keeps the indentation level as it is */                \
        } else if (m->kind & MMK_MATCHER_BIT_CMP) {                            \
            int res = mmk_memcmp(&p->param ## N, &param ## N, sizeof (Type));  \
            if (res == 0 && !(m->kind & MMK_MATCHER_BIT_EQ))                   \
                goto fail;                                                     \
            if (res < 0 && !(m->kind & MMK_MATCHER_BIT_LT))                    \
                goto fail;                                                     \
            if (res > 0 && !(m->kind & MMK_MATCHER_BIT_GT))                    \
                goto fail;                                                     \
        } else if (m->kind == MMK_MATCHER_THAT) {                              \
            int (*predicate)(Type) = (int (*)(Type))                           \
                    mmk_matcher_get_predicate(m);                              \
            if (!predicate(p->param ## N))                                     \
                goto fail;                                                     \
        }                                                                      \
    } else {                                                                   \
        if (mmk_memcmp(&p->param ## N, &param ## N, sizeof (Type)))            \
            goto fail;                                                         \
    }

# define MMK_SET_PARAMS(N, Id, T) bind->params.param ## N = param ## N;

# define MMK_MOCK_DEFINE_TYPES(Id, ...)                                        \
    typedef MMK_VA_HEAD(__VA_ARGS__) MMK_MANGLE(Id, returntype);               \
    typedef MMK_MANGLE(Id, returntype)(*Id)(                                   \
            MMK_EXPAND(MMK_PARAM_LIST(__VA_ARGS__)));                          \
    struct MMK_MANGLE(Id, params) {                                            \
        size_t mmk_times__;                                                    \
        MMK_EXPAND(MMK_APPLY_N(MMK_DEF_FIELD, Id, __VA_ARGS__))                \
    };                                                                         \
    struct MMK_MANGLE(Id, binding) {                                           \
        struct mmk_params item;                                                \
        struct mmk_result result;                                              \
        struct MMK_MANGLE(Id, params) params;                                  \
    };                                                                         \

# define MMK_MOCK_DEFINE(PreDecl, Return, Id, ...)                             \
    MMK_EXPAND(MMK_MOCK_DEFINE_TYPES(Id, __VA_ARGS__))                         \
    static MMK_MANGLE(Id, returntype) MMK_MANGLE(Id, stub)(                    \
        MMK_EXPAND(MMK_PARAM_LIST(__VA_ARGS__)))                               \
    {                                                                          \
        PreDecl(Id);                                                           \
        struct mmk_matcher *matcher_ctx = mmk_matcher_ctx();                   \
        if (matcher_ctx) {                                                     \
            struct mmk_mock_ctx *mock = mmk_stub_context(mmk_ctx ());          \
            if (matcher_ctx->kind == 0) {                                      \
                struct MMK_MANGLE(Id, binding) *bind =                         \
                    mmk_malloc(sizeof (struct MMK_MANGLE(Id, binding)));       \
                bind->result = *mmk_when_get_result();                         \
                MMK_APPLY_N(MMK_SET_PARAMS, Id, __VA_ARGS__)                   \
                mmk_when_impl(mock, bind);                                     \
            } else if (matcher_ctx->kind == 1) {                               \
                size_t times = 0;                                              \
                for (struct MMK_MANGLE(Id, params) *p =                        \
                            mmk_mock_params_begin(mock);                       \
                        p; p = mmk_mock_params_next(mock, p))                  \
                {                                                              \
                    struct mmk_matcher *m = matcher_ctx;                       \
                    (void) m;                                                  \
                    MMK_EXPAND(MMK_APPLY_N(MMK_TRYVERIFY, Id, __VA_ARGS__))    \
                    times += p->mmk_times__;                                   \
                    goto fail;                                                 \
                    fail:;                                                     \
                }                                                              \
                for (struct mmk_matcher *m = matcher_ctx, *next; m;) {         \
                    next = m->next;                                            \
                    mmk_free (m);                                              \
                    m = next;                                                  \
                }                                                              \
                mmk_verify_set_times(times);                                   \
            }                                                                  \
            Return(zero__);                                                    \
        }                                                                      \
        mmk_verify_register_call (&(struct MMK_MANGLE(Id, params)) {           \
                .mmk_times__ = 0,                                              \
                MMK_APPLY_N(MMK_DEF_VERIFY_PARAM, Id, __VA_ARGS__)             \
            }, sizeof (struct MMK_MANGLE(Id, params)));                        \
        struct mmk_params *param = mmk_mock_get_params();                      \
        for (; param; param = param->next) {                                   \
            struct MMK_MANGLE(Id, binding) *bind = (void*) param;              \
            struct mmk_matcher *m = param->matcher_ctx;                        \
            (void) m;                                                          \
            MMK_EXPAND(MMK_APPLY_N(MMK_TRYMATCH, Id, __VA_ARGS__))             \
            if (bind->result.then_errno)                                       \
                errno = bind->result.then_errno;                               \
            if (bind->result.then_return)                                      \
                Return(*(MMK_MANGLE(Id, returntype)*)bind->result.then_return);\
            Return(zero__);                                                    \
        }                                                                      \
        Return(zero__);                                                        \
    }                                                                          \
    static inline Id MMK_MANGLE(Id, create)(const char *tgt) {                 \
        return (Id) mmk_mock_create_internal(tgt,                              \
                (mmk_fn) MMK_MANGLE(Id, stub));                                \
    }                                                                          \
    typedef int MMK_MANGLE(Id, dummy)

# define MMK_MOCK_DEFINE_ZERO(Id) static MMK_MANGLE(Id, returntype) zero__

# define MMK_MOCK_VALUE_RETURN(Val) return (Val)
# define MMK_MOCK_VOID_RETURN(Val) return

# undef mmk_mock_define
# define mmk_mock_define(Id, ...)                                           \
    MMK_EXPAND(MMK_COND_VOID(                                               \
        MMK_MOCK_DEFINE,                                                    \
            (MMK_MOCK_DEFINE_ZERO, MMK_MOCK_VALUE_RETURN, Id, __VA_ARGS__), \
            (MMK_NOOP_FN,          MMK_MOCK_VOID_RETURN,  Id, __VA_ARGS__), \
            MMK_VA_HEAD(__VA_ARGS__)))

#endif /* !MIMICK_MOCK_H_ */
