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
# include <stdarg.h>

# include "matcher.h"
# include "alloc.h"
# include "string.h"
# include "when.h"
# include "verify.h"
# include "va.h"

struct mmk_params {
    struct mmk_matcher *matcher_ctx;
    struct mmk_params *next;
};

struct mmk_mock_options {
    unsigned sentinel_  : 1;
    unsigned noabort    : 1;
};

struct mmk_params *mmk_mock_get_params(void);
void *mmk_mock_params_begin(struct mmk_mock_ctx *mock);
void *mmk_mock_params_next(struct mmk_mock_ctx *mock, void *prev);
mmk_fn mmk_mock_create_internal(const char *target, mmk_fn fn,
        struct mmk_mock_options opts);

void mmk_mock_report_call(void);
void mmk_mock_reset_call(const char *file, int line);

# undef mmk_reset
void mmk_reset(mmk_fn fn);
# define mmk_reset(Fn) mmk_reset((mmk_fn) Fn);

# undef mmk_mock
# define mmk_mock(Target, ...)                      \
    (MMK_MANGLE(MMK_VA_HEAD(__VA_ARGS__), create)(  \
        (Target),                                   \
        (struct mmk_mock_options) {                 \
            .sentinel_ = 1,                         \
            MMK_VA_TAIL(__VA_ARGS__)                \
        }))

# define MMK_MK_ARG_STR(_, X) #X,

# define MMK_MANGLE_(Id, Name) mmkuser_ ## Id ## _ ## Name
# define MMK_MANGLE(Id, Name) MMK_MANGLE_(Id, Name)

# define MMK_DEF_VERIFY_PARAM_VA(Id, ...) \
    MMK_EXPAND(MMK_DEF_VERIFY_PARAM_VA_ ## Id (__VA_ARGS__))
# define MMK_DEF_VERIFY_PARAM_VA_WITH(...)

# define MMK_DEF_VERIFY_PARAM__(X) .X = X,
# define MMK_DEF_VERIFY_PARAM_VA_WITHOUT(N, Id, T, ...) \
    MMK_DEF_VERIFY_PARAM__(param ## N)

# define MMK_DEF_VERIFY_PARAM_(N, Id, T) \
    MMK_COND_VA(MMK_DEF_VERIFY_PARAM_VA, (WITHOUT, N, Id, T,), (WITH,), T)
# define MMK_DEF_VERIFY_PARAM(N, Id, T) \
    MMK_EXPAND(MMK_DEF_VERIFY_PARAM_(N, Id, T))

# define MMK_DEF_FIELD_VA(Id, ...) \
    MMK_EXPAND(MMK_DEF_FIELD_VA_ ## Id (__VA_ARGS__))
# define MMK_DEF_FIELD_VA_WITH(...)
# define MMK_DEF_FIELD_VA_WITHOUT(N, T, ...) T param ## N;

# define MMK_DEF_FIELD_(N, T) \
    MMK_COND_VA(MMK_DEF_FIELD_VA, (WITHOUT, N, T,), (WITH,), T)
# define MMK_DEF_FIELD(N, _, T) MMK_EXPAND(MMK_DEF_FIELD_(N, T))

# define MMK_TRYMATCH_VA(Id, ...) \
    MMK_EXPAND(MMK_TRYMATCH_VA_ ## Id (__VA_ARGS__))
# define MMK_TRYMATCH_VA_WITH(...)

# define MMK_TRYMATCH_(N, Name, T) \
    MMK_COND_VA(MMK_TRYMATCH_VA, (WITHOUT, N, Name, T,), (WITH,), T)
# define MMK_TRYMATCH(N, Name, T) MMK_EXPAND(MMK_TRYMATCH_(N, Name, T))

# define MMK_TRYMATCH_VA_WITHOUT(N, Name, Type, ...)                           \
    if (m->next) {                                                             \
        m = m->next;                                                           \
        if (m->kind == MMK_MATCHER_ANY) {                                      \
            /* Ugly but keeps the indentation level as it is */                \
        } else if (m->kind & MMK_MATCHER_BIT_CMP) {                            \
            if (param ## N == bind->params.param ## N                          \
                    && !(m->kind & MMK_MATCHER_BIT_EQ))                        \
                continue;                                                      \
            if (param ## N < bind->params.param ## N                           \
                    && !(m->kind & MMK_MATCHER_BIT_LT))                        \
                continue;                                                      \
            if (param ## N > bind->params.param ## N                           \
                    && !(m->kind & MMK_MATCHER_BIT_GT))                        \
                continue;                                                      \
        } else if (m->kind == MMK_MATCHER_THAT) {                              \
            int (*predicate)(Type) = (int (*)(Type))                           \
                    mmk_matcher_get_predicate(m);                              \
            if (!predicate(param ## N))                                        \
                continue;                                                      \
        }                                                                      \
    } else if (param ## N != bind->params.param ## N) {                        \
        continue;                                                              \
    }

# define MMK_TRYVERIFY_VA(Id, ...) \
    MMK_EXPAND(MMK_TRYVERIFY_VA_ ## Id (__VA_ARGS__))

# define MMK_TRYVERIFY_VA_WITH(...)

# define MMK_TRYVERIFY_(N, Name, T) \
    MMK_COND_VA(MMK_TRYVERIFY_VA, (WITHOUT, N, Name, T,), (WITH,), T)

# define MMK_TRYVERIFY(N, Name, T) MMK_EXPAND(MMK_TRYVERIFY_(N, Name, T))

# define MMK_TRYVERIFY_VA_WITHOUT(N, Name, Type, ...)                          \
    if (m->next) {                                                             \
        m = m->next;                                                           \
        if (m->kind == MMK_MATCHER_ANY) {                                      \
            /* Ugly but keeps the indentation level as it is */                \
        } else if (m->kind & MMK_MATCHER_BIT_CMP) {                            \
            if (p->param ## N == param ## N                                    \
                    && !(m->kind & MMK_MATCHER_BIT_EQ))                        \
                goto fail;                                                     \
            if (p->param ## N < param ## N                                     \
                    && !(m->kind & MMK_MATCHER_BIT_LT))                        \
                goto fail;                                                     \
            if (p->param ## N > param ## N                                     \
                    && !(m->kind & MMK_MATCHER_BIT_GT))                        \
                goto fail;                                                     \
        } else if (m->kind == MMK_MATCHER_THAT) {                              \
            int (*predicate)(Type) = (int (*)(Type))                           \
                    mmk_matcher_get_predicate(m);                              \
            if (!predicate(p->param ## N))                                     \
                goto fail;                                                     \
        }                                                                      \
    } else if (p->param ## N != param ## N) {                                  \
        goto fail;                                                             \
    }

# define MMK_SET_PARAMS_VA(Id, ...) \
    MMK_EXPAND(MMK_SET_PARAMS_VA_ ## Id (__VA_ARGS__))

# define MMK_SET_PARAMS_VA_WITH(...)
# define MMK_SET_PARAMS_VA_WITHOUT(N, Id, T, ...) \
    bind->params.param ## N = param ## N;

# define MMK_SET_PARAMS_(N, Id, T) \
    MMK_COND_VA(MMK_SET_PARAMS_VA, (WITHOUT, N, Id, T,), (WITH,), T)

# define MMK_SET_PARAMS(N, Id, T) MMK_EXPAND(MMK_SET_PARAMS_(N, Id, T))

# define MMK_VA_TYPE(...) ...
# define MMK_VA_LIST_TYPE(...) va_list vl__
# define MMK_VA_LIST_TYPE_SRLZ(...) va_list vl__, \
    size_t *mmk_size_out, struct mmk_va_param ***data_out

# define MMK_MOCK_DEFINE_TYPES(Id, ...)                                        \
    typedef MMK_VA_HEAD(__VA_ARGS__) MMK_MANGLE(Id, returntype);               \
    typedef MMK_MANGLE(Id, returntype)(*Id)(                                   \
            MMK_EXPAND(MMK_PARAM_LIST(MMK_VA_TYPE, __VA_ARGS__)));             \
    typedef MMK_MANGLE(Id, returntype)(*MMK_MANGLE(Id, delegate))(             \
            MMK_EXPAND(MMK_PARAM_LIST(MMK_VA_LIST_TYPE, __VA_ARGS__)));        \
    typedef int (*MMK_MANGLE(Id, va_serializer))(                              \
            MMK_EXPAND(MMK_PARAM_LIST(MMK_VA_LIST_TYPE_SRLZ, __VA_ARGS__)));   \
    struct MMK_MANGLE(Id, params) {                                            \
        size_t mmk_times__;                                                    \
        size_t mmk_va_args_sz__;                                               \
        struct mmk_va_param **mmk_va_args__;                                   \
        MMK_EXPAND(MMK_APPLY_N(MMK_DEF_FIELD, Id, __VA_ARGS__))                \
    };                                                                         \
    struct MMK_MANGLE(Id, binding) {                                           \
        struct mmk_params item;                                                \
        struct mmk_result result;                                              \
        struct MMK_MANGLE(Id, params) params;                                  \
    };                                                                         \

# define MMK_MOCK_VA_PREDECL(Id, ...) \
    MMK_MANGLE(Id, va_serializer) mmk_va_serializer__ =                 \
        MMK_MANGLE(Id, serialize_va);                                   \
    va_list vl__, vl_cpy__;                                             \
    va_start(vl__, MMK_PARAM_N(param,                                   \
            MMK_DEC(MMK_DEC(MMK_VA_NARGS(MMK_VA_TAIL(__VA_ARGS__)))))   \
    )

# define MMK_MOCK_VA_COPY(...) \
    va_copy(vl_cpy__, vl__)

# define MMK_MOCK_VA_COPY_END(...) \
    va_end(vl_cpy__)

# define MMK_MOCK_VA_END(...) \
    va_end(vl__)

# define MMK_MOCK_VA_BIND(Params, ...) do {                                    \
        size_t mmk_va_args_sz__ = 0;                                           \
        struct mmk_va_param **mmk_va_args__ = NULL;                            \
        MMK_MOCK_VA_COPY(_);                                                   \
        int mmk_ok__ =                                                         \
            mmk_va_serializer__(MMK_EXPAND(MMK_PARAM_PACK(__VA_ARGS__)),       \
                    &mmk_va_args_sz__, &mmk_va_args__);                        \
        MMK_MOCK_VA_COPY_END(_);                                               \
        if (mmk_ok__) {                                                        \
            (Params)->mmk_va_args_sz__ = mmk_va_args_sz__;                     \
            (Params)->mmk_va_args__ = mmk_va_args__;                           \
        }                                                                      \
    } while (0)

# define MMK_MOCK_VA_VERIFY(Id, Params, ...) do {                              \
        size_t mmk_va_args_sz__ = 0;                                           \
        struct mmk_va_param **mmk_va_args__ = NULL;                            \
        MMK_MOCK_VA_COPY(_);                                                   \
        int mmk_ok__ =                                                         \
            mmk_va_serializer__(MMK_EXPAND(MMK_PARAM_PACK(__VA_ARGS__)),       \
                    &mmk_va_args_sz__, &mmk_va_args__);                        \
        MMK_MOCK_VA_COPY_END(_);                                               \
        if (!mmk_ok__ || (Params)->mmk_va_args_sz__ != mmk_va_args_sz__)       \
            goto fail;                                                         \
        for (size_t i = 0; i < mmk_va_args_sz__; ++i) {                        \
            struct mmk_va_param *mmk_vp_ref = (Params)->mmk_va_args__[i];      \
            struct mmk_va_param *mmk_vp = mmk_va_args__[i];                    \
            if (mmk_vp_ref->size != mmk_vp->size)                              \
                goto fail;                                                     \
            if (m->next) {                                                     \
                m = m->next;                                                   \
                if (m->kind == MMK_MATCHER_ANY) {                              \
                    /* Ugly but keeps the indentation level as it is */        \
                } else if (m->kind & MMK_MATCHER_BIT_CMP) {                    \
                    int res = mmk_memcmp(mmk_vp_ref->data,                     \
                            mmk_vp->data, mmk_vp->size);                       \
                    if (res == 0 && !(m->kind & MMK_MATCHER_BIT_EQ))           \
                        goto fail;                                             \
                    if (res < 0 && !(m->kind & MMK_MATCHER_BIT_LT))            \
                        goto fail;                                             \
                    if (res > 0 && !(m->kind & MMK_MATCHER_BIT_GT))            \
                        goto fail;                                             \
                } else if (m->kind == MMK_MATCHER_THAT) {                      \
                    int (*predicate)(struct mmk_va_param *) =                  \
                        (int (*)(struct mmk_va_param *))                       \
                            mmk_matcher_get_predicate(m);                      \
                    if (!predicate(mmk_vp_ref))                                \
                        goto fail;                                             \
                }                                                              \
            } else {                                                           \
                if (mmk_memcmp(mmk_vp_ref->data, mmk_vp->data, mmk_vp->size))  \
                    goto fail;                                                 \
            }                                                                  \
            mmk_free(mmk_vp);                                                  \
        }                                                                      \
        mmk_free(mmk_va_args__);                                               \
    } while (0)

# define MMK_MOCK_VA_REGISTER_CALL(RegParams, ...) do {                        \
        size_t mmk_va_args_sz__ = 0;                                           \
        struct mmk_va_param **mmk_va_args__ = NULL;                            \
        MMK_MOCK_VA_COPY(_);                                                   \
        int mmk_ok__ =                                                         \
            mmk_va_serializer__(MMK_EXPAND(MMK_PARAM_PACK(__VA_ARGS__)),       \
                    &mmk_va_args_sz__, &mmk_va_args__);                        \
        MMK_MOCK_VA_COPY_END(_);                                               \
        if (mmk_ok__) {                                                        \
            (RegParams)->mmk_va_args_sz__ = mmk_va_args_sz__;                  \
            (RegParams)->mmk_va_args__ = mmk_va_args__;                        \
        }                                                                      \
    } while (0)

# define MMK_MOCK_DEFINE(PreDecl, Return, ReturnSE, VaMacro, Id, ...)          \
    MMK_EXPAND(MMK_MOCK_DEFINE_TYPES(Id, __VA_ARGS__))                         \
    static MMK_MANGLE(Id, returntype) MMK_MANGLE(Id, stub)(                    \
        MMK_EXPAND(MMK_PARAM_LIST(MMK_VA_TYPE, __VA_ARGS__)))                  \
    {                                                                          \
        VaMacro(PREDECL)(Id, __VA_ARGS__);                                     \
        PreDecl(Id);                                                           \
        struct mmk_matcher *matcher_ctx = mmk_matcher_ctx();                   \
        if (matcher_ctx) {                                                     \
            struct mmk_mock_ctx *mock = mmk_stub_context(mmk_ctx ());          \
            mmk_mock_report_call();                                            \
            if (matcher_ctx->kind == 0) {                                      \
                struct MMK_MANGLE(Id, binding) *bind =                         \
                    mmk_malloc(sizeof (struct MMK_MANGLE(Id, binding)));       \
                bind->result = *mmk_when_get_result();                         \
                MMK_APPLY_N(MMK_SET_PARAMS, Id, __VA_ARGS__)                   \
                MMK_EXPAND(VaMacro(BIND)(&bind->params, __VA_ARGS__));         \
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
                    MMK_EXPAND(VaMacro(VERIFY)(Id, p, __VA_ARGS__));           \
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
            VaMacro(END)(_);                                                   \
            Return(zero__);                                                    \
        }                                                                      \
        struct MMK_MANGLE(Id, params) reg_params = {                           \
                .mmk_times__ = 0,                                              \
                MMK_APPLY_N(MMK_DEF_VERIFY_PARAM, Id, __VA_ARGS__)             \
            };                                                                 \
        MMK_EXPAND(VaMacro(REGISTER_CALL)(&reg_params, __VA_ARGS__));          \
        mmk_verify_register_call(&reg_params, sizeof (reg_params));            \
        struct mmk_params *param = mmk_mock_get_params();                      \
        for (; param; param = param->next) {                                   \
            struct MMK_MANGLE(Id, binding) *bind = (void*) param;              \
            struct mmk_matcher *m = param->matcher_ctx;                        \
            (void) m;                                                          \
            MMK_EXPAND(MMK_APPLY_N(MMK_TRYMATCH, Id, __VA_ARGS__))             \
            if (bind->result.then_errno)                                       \
                errno = bind->result.then_errno;                               \
            if (bind->result.then_call && !bind->result.then_return) {         \
                ReturnSE(Id, VaMacro,                                          \
                    ((MMK_MANGLE(Id, delegate)) bind->result.then_call)(       \
                        MMK_EXPAND(MMK_PARAM_PACK(__VA_ARGS__))                \
                ));                                                            \
            }                                                                  \
            if (bind->result.then_call && bind->result.then_return) {          \
                VaMacro(COPY)(_);                                              \
                (void) ((MMK_MANGLE(Id, delegate)) bind->result.then_call)(    \
                    MMK_EXPAND(MMK_PARAM_PACK(__VA_ARGS__))                    \
                );                                                             \
                VaMacro(COPY_END)(_);                                          \
                VaMacro(END)(_);                                               \
                Return(*(MMK_MANGLE(Id, returntype)*)bind->result.then_return);\
            }                                                                  \
            if (bind->result.then_return) {                                    \
                VaMacro(END)(_);                                               \
                Return(*(MMK_MANGLE(Id, returntype)*)bind->result.then_return);\
            }                                                                  \
            VaMacro(END)(_);                                                   \
            Return(zero__);                                                    \
        }                                                                      \
        VaMacro(END)(_);                                                       \
        Return(zero__);                                                        \
    }                                                                          \
    static inline Id MMK_MANGLE(Id, create)(const char *tgt,                   \
            struct mmk_mock_options opts)                                      \
    {                                                                          \
        return (Id) mmk_mock_create_internal(tgt,                              \
                (mmk_fn) MMK_MANGLE(Id, stub), opts);                          \
    }                                                                          \
    typedef int MMK_MANGLE(Id, dummy)

# define MMK_MOCK_DEFINE_ZERO(Id) static MMK_MANGLE(Id, returntype) zero__

# define MMK_MOCK_VALUE_RETURN(Val) return (Val)
# define MMK_MOCK_VALUE_RETURN_SE(Id, VaMacro, Val) do {    \
        VaMacro(COPY)(_);                                   \
        MMK_MANGLE(Id, returntype) mmk_val__ = (Val);       \
        VaMacro(COPY_END)(_);                               \
        VaMacro(END)(_);                                    \
        return mmk_val__;                                   \
    } while (0)

# define MMK_MOCK_VOID_RETURN(Val) return
# define MMK_MOCK_VOID_RETURN_SE(Id, VaMacro, Val) do {     \
        VaMacro(COPY)(_);                                   \
        (void) (Val);                                       \
        VaMacro(COPY_END)(_);                               \
        VaMacro(END)(_);                                    \
        return;                                             \
    } while (0)

# define MMK_MOCK_VA(Id) MMK_MOCK_VA_ ## Id
# define MMK_MOCK_VA_WITH(Id) MMK_EXPAND(MMK_MOCK_VA_ ## Id)
# define MMK_MOCK_VA_WITHOUT(Id) MMK_NOOP_FN

# undef mmk_mock_define
# define mmk_mock_define(Id, ...)                           \
    MMK_EXPAND(MMK_MOCK_DEFINE_IMPL(Id,                     \
        MMK_EXPAND(MMK_COND_VA(                             \
                MMK_MOCK_VA,                                \
                (WITHOUT),                                  \
                (WITH),                                     \
                MMK_LAST(__VA_ARGS__))),                    \
        __VA_ARGS__))

# define MMK_MOCK_DEFINE_IMPL(Id, VaMacro, ...)             \
    MMK_EXPAND(MMK_COND_VOID(                               \
        MMK_MOCK_DEFINE,                                    \
            (MMK_MOCK_DEFINE_ZERO,                          \
                MMK_MOCK_VALUE_RETURN,                      \
                MMK_MOCK_VALUE_RETURN_SE,                   \
                VaMacro,                                    \
                Id, __VA_ARGS__),                           \
            (MMK_NOOP_FN,                                   \
                MMK_MOCK_VOID_RETURN,                       \
                MMK_MOCK_VOID_RETURN_SE,                    \
                VaMacro,                                    \
                Id, __VA_ARGS__),                           \
            MMK_VA_HEAD(__VA_ARGS__)))

#endif /* !MIMICK_MOCK_H_ */
