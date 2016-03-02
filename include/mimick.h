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
#ifndef MIMICK_H_
# define MIMICK_H_

# include <stdint.h>
# include <stdbool.h>
# include <string.h>
# include "mimick/preprocess.h"
# include "mimick/item.h"
# include "mimick/offset.h"

typedef void (*mmk_fn)(void);

typedef struct mmk_mock *mmk_mock;
typedef struct mmk_stub *mmk_stub;

void *mmk_stub_context (mmk_stub stub);
mmk_stub mmk_stub_create (const char *name, const char *path, mmk_fn fn, void *ctx);
void mmk_stub_destroy (mmk_stub stub);

extern mmk_stub mmk_ctx;

void mmk_init (void);
mmk_mock mmk_mock_create (const char *name, const char *path, struct mmk_offset *offsets, mmk_fn fn);
void mmk_mock_destroy (mmk_mock mock);
void mmk_bind (mmk_mock mock, const char **params_str, void *params);

# define mmk_mock_create(Name, Path, Id) mmk_mock_create((Name), (Path), Id ## _offsets_, (mmk_fn) Id)

struct mmk_item *mmk_pop_params (void);

# define MMK_MK_ARG_STR_(X) #X
# define MMK_MK_ARG_STR(_, X) MMK_MK_ARG_STR_(X),

# define mmk_expect(Mock, Name, ...) mmk_bind((Mock),                        \
        (const char *[]) { MMK_APPLY(MMK_MK_ARG_STR, _, __VA_ARGS__) NULL }, \
        &(struct Name ## _params_) { __VA_ARGS__ }                           \
    )

# define MMK_DEF_PRESENT_(X) bool X ## _present_;
# define MMK_DEF_PRESENT(_, T, X) MMK_DEF_PRESENT_(X)

# define MMK_DEF_FIELD_(T, X) T X;
# define MMK_DEF_FIELD(_, T, X) MMK_DEF_FIELD_(T, X)

# define MMK_DEF_OFFSET_(Name, T, X) { \
        #X, \
        offsetof (struct Name ## _params_, X), \
        offsetof (struct Name ## _params_, X ## _present_) \
    },
# define MMK_DEF_OFFSET(Name, T, X) MMK_DEF_OFFSET_(Name, T, X)

# define MMK_DEF_ASSERT_(Var, X) if ((Var)->X ## _present_) assert (X == (Var)->X);
# define MMK_DEF_ASSERT(Var, T, X) MMK_DEF_ASSERT_(Var, X)

# define mmk_mock_define(Name, ReturnType, ...) \
    struct Name ## _params_ { \
        struct mmk_item it_; \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_PRESENT, _, __VA_ARGS__)) \
        ReturnType returning; \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_FIELD, Name, __VA_ARGS__)) \
    }; \
    extern struct mmk_offset Name ## _offsets_[]; \
    static inline ReturnType Name(void *ptr, size_t size) { \
        struct mmk_item *it = mmk_pop_params (); \
        assert (it != NULL); \
        struct Name ## _params_ *params = mmk_cont(it, struct Name ## _params_, it_); \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_ASSERT, params, __VA_ARGS__)) \
        return params->returning; \
    } \
    struct mmk_offset Name ## _offsets_[] = { \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_OFFSET, Name, __VA_ARGS__)) \
        { NULL, 0, 0 } \
    }

# define mmk_mock_define_void(Name, ...) \
    struct Name ## _params_ { \
        struct mmk_item it_; \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_PRESENT, _, __VA_ARGS__)) \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_FIELD, Name, __VA_ARGS__)) \
    }; \
    extern struct mmk_offset Name ## _offsets_[]; \
    static inline void Name(void *ptr, size_t size) { \
        struct mmk_item *it = mmk_pop_params (); \
        assert (it != NULL); \
        struct Name ## _params_ *params = mmk_cont(it, struct Name ## _params_, it_); \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_ASSERT, params, __VA_ARGS__)) \
    } \
    struct mmk_offset Name ## _offsets_[] = { \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_OFFSET, Name, __VA_ARGS__)) \
        { NULL, 0, 0 } \
    }

#endif /* !MIMICK_H_ */
