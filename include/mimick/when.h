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
#ifndef MMK_WHEN_H_
# define MMK_WHEN_H_

# include "literal.h"
# include "va.h"

# ifdef __cplusplus
extern "C" {
# endif

struct mmk_mock_ctx;

struct mmk_result {
    int sentinel_;
    int then_errno;
    void *then_return;
    void (*then_call)(void);
    struct mmk_va_info *with_va;
};

void mmk_when_init(struct mmk_result *res);
void mmk_when_impl(struct mmk_mock_ctx *mock, void *data);
struct mmk_result *mmk_when_get_result(void);

# ifdef __cplusplus
}
# endif

# undef mmk_when
# define mmk_when(CallExpr, ...) \
        (mmk_matcher_init(0), \
         mmk_when_init(&mmk_struct_literal( \
             struct mmk_result, \
             __VA_ARGS__, \
             .sentinel_ = 0)), \
         (void) (CallExpr), \
         mmk_mock_reset_call(__FILE__, __LINE__), \
         mmk_matcher_term())

#endif /* !MMK_WHEN_H_ */
