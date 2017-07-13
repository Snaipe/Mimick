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
#ifndef MIMICK_MATCHER_H_
# define MIMICK_MATCHER_H_

# include <stddef.h>

/* Using anonymous structs in compound literals is perfectly valid C,
   and we use this behaviour in Mimick. This MSVC warning does nothing
   useful in that regard, so we simply disable it. */
# ifdef _MSC_VER
#  pragma warning (disable: 4116)
# endif

# define MMK_MATCHER_BIT_CMP (1 << (8 * sizeof (int) - 2))
# define MMK_MATCHER_BIT_EQ (1 << 1)
# define MMK_MATCHER_BIT_LT (1 << 2)
# define MMK_MATCHER_BIT_GT (1 << 3)

enum mmk_matcher_kind {
    MMK_MATCHER_ANY,
    MMK_MATCHER_THAT,

    MMK_MATCHER_EQ  = MMK_MATCHER_BIT_CMP | MMK_MATCHER_BIT_EQ,
    MMK_MATCHER_NEQ = MMK_MATCHER_BIT_CMP | MMK_MATCHER_BIT_LT
                    | MMK_MATCHER_BIT_GT,
    MMK_MATCHER_LT  = MMK_MATCHER_BIT_CMP | MMK_MATCHER_BIT_LT,
    MMK_MATCHER_LEQ = MMK_MATCHER_BIT_CMP | MMK_MATCHER_BIT_EQ
                    | MMK_MATCHER_BIT_LT,
    MMK_MATCHER_GT  = MMK_MATCHER_BIT_CMP | MMK_MATCHER_BIT_GT,
    MMK_MATCHER_GEQ = MMK_MATCHER_BIT_CMP | MMK_MATCHER_BIT_EQ
                    | MMK_MATCHER_BIT_GT,
};

struct mmk_matcher {
    enum mmk_matcher_kind kind;
    size_t prio;
    void *data;
    struct mmk_matcher *next;
};

# define mmk_matcher_val_(Kind, Type, Val) (mmk_matcher_add(Kind, __COUNTER__), ((Type) Val))
# undef mmk_any
# define mmk_any(Type)      mmk_matcher_val_(MMK_MATCHER_ANY, Type, { 0 })
# undef mmk_eq
# define mmk_eq(Type, Val)  mmk_matcher_val_(MMK_MATCHER_EQ, Type, Val)
# undef mmk_ne
# define mmk_ne(Type, Val) mmk_matcher_val_(MMK_MATCHER_NEQ, Type, Val)
# undef mmk_lt
# define mmk_lt(Type, Val)  mmk_matcher_val_(MMK_MATCHER_LT, Type, Val)
# undef mmk_le
# define mmk_le(Type, Val) mmk_matcher_val_(MMK_MATCHER_LEQ, Type, Val)
# undef mmk_gt
# define mmk_gt(Type, Val)  mmk_matcher_val_(MMK_MATCHER_GT, Type, Val)
# undef mmk_ge
# define mmk_ge(Type, Val) mmk_matcher_val_(MMK_MATCHER_GEQ, Type, Val)
# undef mmk_that
# define mmk_that(Type, Predicate) (mmk_matcher_add_fn(MMK_MATCHER_THAT, __COUNTER__, (void(*)(void))*(int(*[1])(Type)) {(Predicate)}), ((Type) { 0 }))

void mmk_matcher_init(int kind);
void mmk_matcher_add(enum mmk_matcher_kind kind, int counter);
void mmk_matcher_add_fn(enum mmk_matcher_kind kind, int counter, void (*fn)(void));
void mmk_matcher_add_data(enum mmk_matcher_kind kind, int counter, void *data);
void mmk_matcher_term(void);
struct mmk_matcher *mmk_matcher_ctx(void);
void (*mmk_matcher_get_predicate(struct mmk_matcher *m))(void);

#endif /* !MIMICK_MATCHER_H_ */
