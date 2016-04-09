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

# include "string.h"
# include "../mimick.h"

/* mmk_va_args is actually just a preprocessed token without any real
   definition -- we just reserve it anyway in mimick.h. */
# undef mmk_va_args

# define mmk_make_va_param(vd, vl, type) do {                               \
        (vd) = mmk_malloc(sizeof (struct mmk_va_param) + sizeof (type));    \
        (vd)->size = sizeof (type);                                         \
        type val = va_arg(vl, type);                                        \
        mmk_memcpy((vd)->data, &val, sizeof (type));                        \
    } while (0)

#endif /* !MMK_VA_H_ */
