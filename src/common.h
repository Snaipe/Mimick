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
#ifndef COMMON_H_
# define COMMON_H_

# include <stdlib.h>

# if __STDC_VERSION__ >= 201112L
#  include <stdnoreturn.h>
#  define mmk_noreturn _Noreturn
# elif defined __GNUC__
#  define mmk_noreturn __attribute__((noreturn))
# elif defined _MSC_VER
#  define mmk_noreturn __declspec(noreturn)
# else
#  define mmk_noreturn
# endif

# if defined __GNUC__
#  define mmk_unreachable() __builtin_unreachable()
# elif defined _MSC_VER
#  define mmk_unreachable() __assume(0)
# else
/* Last resort. We *need* to theoritically call a noreturn function after
 * mmk_abort, so the function itself can be marked noreturn.
 * In reality, this is never called and is just a bunch of wasted opcodes. */
#  define mmk_unreachable() abort()
# endif

#endif /* !COMMON_H_ */
