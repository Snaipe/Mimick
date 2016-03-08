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

# include "offset.h"

enum mmk_matcher_kind {
    MMK_MATCHER_ANY,
    MMK_MATCHER_NEQ,
    MMK_MATCHER_LT,
    MMK_MATCHER_LEQ,
    MMK_MATCHER_GT,
    MMK_MATCHER_GEQ,
    MMK_MATCHER_THAT,
};

# define MMK_MATCHER_CMP_START MMK_MATCHER_NEQ
# define MMK_MATCHER_CMP_END MMK_MATCHER_GEQ

struct mmk_matcher {
    enum mmk_matcher_kind kind;
    size_t prio;
    struct mmk_matcher *next;
};

# define MMK_MATCHER_INIT(Type) ((struct mmk_matcher *) &(struct { struct mmk_matcher matcher; Type val; }) { .matcher = { .kind = 0 } })

# define mmk_any(Type) (mmk_matcher_add(MMK_MATCHER_ANY, __COUNTER__, MMK_MATCHER_INIT(Type)), (Type) { 0 })

# define mmk_matcher_val_(Kind, Type, Val) (mmk_matcher_add(Kind, __COUNTER__, MMK_MATCHER_INIT(Type)), ((Type) Val))
# define mmk_neq(Type, Val) mmk_matcher_val_(MMK_MATCHER_NEQ, Type, Val)
# define mmk_lt(Type, Val)  mmk_matcher_val_(MMK_MATCHER_LT, Type, Val)
# define mmk_leq(Type, Val) mmk_matcher_val_(MMK_MATCHER_LEQ, Type, Val)
# define mmk_gt(Type, Val)  mmk_matcher_val_(MMK_MATCHER_GT, Type, Val)
# define mmk_geq(Type, Val) mmk_matcher_val_(MMK_MATCHER_GEQ, Type, Val)

# define mmk_that(Predicate) ((struct mmk_matcher *) &(struct { struct mmk_matcher matcher; void (*val)(void); }) { .val = (void (*)(void)) Predicate })

void mmk_matcher_init(int counter, struct mmk_matcher *ctx, char *callexpr);
void mmk_matcher_init_verify(struct mmk_matcher *ctx, const char **order, char **params);
void mmk_matcher_add(enum mmk_matcher_kind kind, int counter, struct mmk_matcher *out);
void mmk_matcher_term(void);
struct mmk_matcher *mmk_matcher_ctx(void);

int mmk_memcmp(const void *s1, const void *s2, size_t n);
void (*mmk_matcher_get_predicate(struct mmk_matcher *m))(void);

#endif /* !MIMICK_MATCHER_H_ */
