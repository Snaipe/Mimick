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
#ifndef MMK_VA_H_
# define MMK_VA_H_

struct mmk_va_info {
    size_t nb_args;
    size_t *types;
};

# define MMK_SIZEOF_T(N, _, T) sizeof (T),

# define MMK_VA_IMPL(N, ...) &(struct mmk_va_info) {                        \
        .nb_args = N,                                                       \
        .types = &(size_t[N]) { MMK_APPLY_N(MMK_SIZEOF_T, _, __VA_ARGS__) } \
    }

# undef mmk_va
# define mmk_va(...) MMK_VA_IMPL(MMK_VA_NARGS(__VA_ARGS__), __VA_ARGS__)

#endif /* !MMK_VA_H_ */
