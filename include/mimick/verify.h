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
#ifndef MMK_VERIFY_H_
# define MMK_VERIFY_H_

# include <stddef.h>
# include "common.h"

struct mmk_verify_params {
    int at_least_once;
    int never;
    size_t times;
    int (*that)(size_t);
};

void mmk_verify_set_times(size_t times);
int mmk_verify_times(struct mmk_verify_params *params);
void mmk_verify_register_call (void *params, size_t size);

# undef mmk_verify
# define mmk_verify(CallExpr, ...) \
        (mmk_matcher_init(1, #CallExpr), \
        mmk_use_result__(0, CallExpr), \
        mmk_matcher_term (), \
        mmk_verify_times(&(struct mmk_verify_params) { __VA_ARGS__ }))

#endif /* !MMK_VERIFY_H_ */
