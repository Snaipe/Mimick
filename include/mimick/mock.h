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

# undef mmk_mock_create
# define mmk_mock_create(Target, Id) mmk_mock_create_internal((Target), Id ## _offsets_, (mmk_fn) Id)

# define MMK_MK_ARG_STR_(X) #X
# define MMK_MK_ARG_STR(_, X) MMK_MK_ARG_STR_(X),

# define MMK_MANGLE_(Id, Name) Id ## _ ## Name ## _
# define MMK_MANGLE(Id, Name) MMK_MANGLE_(Id, Name)

# undef mmk_expect
# define mmk_expect(Id, Mock, ...) mmk_bind((Mock),                          \
        (const char *[]) { MMK_APPLY(MMK_MK_ARG_STR, _, __VA_ARGS__) NULL }, \
        &(struct MMK_MANGLE(Id, params)) { __VA_ARGS__ }                     \
    )

# define MMK_DEF_PRESENT_(X) bool X ## _present_;
# define MMK_DEF_PRESENT(_, T, X) MMK_DEF_PRESENT_(X)

# define MMK_DEF_FIELD_(T, X) T X;
# define MMK_DEF_FIELD(_, T, X) MMK_DEF_FIELD_(T, X)

# define MMK_DEF_OFFSET_(Name, T, X) { \
        #X, \
        offsetof (struct MMK_MANGLE(Name, params), X), \
        offsetof (struct MMK_MANGLE(Name, params), X ## _present_) \
    },
# define MMK_DEF_OFFSET(Name, T, X) MMK_DEF_OFFSET_(Name, T, X)

# define MMK_DEF_ASSERT_(Var, X) if ((Var)->X ## _present_) assert (X == (Var)->X);
# define MMK_DEF_ASSERT(Var, T, X) MMK_DEF_ASSERT_(Var, X)

# undef mmk_mock_define
# define mmk_mock_define(Name, ...) \
    struct MMK_MANGLE(Name, params) { \
        struct mmk_item it_; \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_PRESENT, _, MMK_VA_TAIL(__VA_ARGS__))) \
        bool times_present_; \
        long times; \
        MMK_VA_HEAD(__VA_ARGS__) returning; \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_FIELD, Name, MMK_VA_TAIL(__VA_ARGS__))) \
    }; \
    extern struct mmk_offset MMK_MANGLE(Name, offsets)[]; \
    static inline MMK_VA_HEAD(__VA_ARGS__) Name(MMK_EXPAND(MMK_PARAM_LIST(MMK_VA_TAIL(__VA_ARGS__)))) { \
        struct mmk_item *it = mmk_pop_params (); \
        assert (it != NULL); \
        struct MMK_MANGLE(Name, params) *params = mmk_cont(it, struct MMK_MANGLE(Name, params), it_); \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_ASSERT, params, MMK_VA_TAIL(__VA_ARGS__))) \
        return params->returning; \
    } \
    struct mmk_offset MMK_MANGLE(Name, offsets)[] = { \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_OFFSET, Name, MMK_VA_TAIL(__VA_ARGS__))) \
        MMK_DEF_OFFSET(Name, _, times) \
        { NULL, 0, 0 } \
    }

# undef mmk_mock_define_void
# define mmk_mock_define_void(...) \
    struct MMK_MANGLE(MMK_VA_HEAD(__VA_ARGS__), params) { \
        struct mmk_item it_; \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_PRESENT, _, MMK_VA_TAIL(__VA_ARGS__))) \
        bool times_present_; \
        long times; \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_FIELD, MMK_VA_HEAD(__VA_ARGS__), MMK_VA_TAIL(__VA_ARGS__))) \
    }; \
    extern struct mmk_offset MMK_MANGLE(MMK_VA_HEAD(__VA_ARGS__), offsets)[]; \
    static inline void MMK_VA_HEAD(__VA_ARGS__)(MMK_EXPAND(MMK_PARAM_LIST(MMK_VA_TAIL(__VA_ARGS__)))) { \
        struct mmk_item *it = mmk_pop_params (); \
        assert (it != NULL); \
        struct MMK_MANGLE(MMK_VA_HEAD(__VA_ARGS__), params) *params = mmk_cont(it, struct MMK_MANGLE(MMK_VA_HEAD(__VA_ARGS__), params), it_); \
        (void) params; \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_ASSERT, params, MMK_VA_TAIL(__VA_ARGS__))) \
    } \
    struct mmk_offset MMK_MANGLE(MMK_VA_HEAD(__VA_ARGS__), offsets)[] = { \
        MMK_EXPAND(MMK_PAIR_APPLY(MMK_DEF_OFFSET, MMK_VA_HEAD(__VA_ARGS__), MMK_VA_TAIL(__VA_ARGS__))) \
        MMK_DEF_OFFSET(MMK_VA_HEAD(__VA_ARGS__), _, times) \
        { NULL, 0, 0 } \
    }

mmk_mock mmk_mock_create_internal (const char *target, struct mmk_offset *offsets, mmk_fn fn);
struct mmk_item *mmk_pop_params (void);
void mmk_bind (mmk_mock mock, const char **params_str, void *params);

#endif /* !MIMICK_MOCK_H_ */
