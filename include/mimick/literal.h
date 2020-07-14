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
#ifndef MMK_LITERAL_H_
# define MMK_LITERAL_H_

# undef mmk_struct_literal

# ifdef __cplusplus

#  include "preprocess.h"

template<typename T, int ID>
struct mmk_literal {
  static constexpr int id = ID;
  static T storage;
};

template<typename T, int ID>
T mmk_literal<T, ID>::storage{0};

#  define MMK_COMMA_APPEND(_, prefix, content) (void) (prefix content),

#  define mmk_struct_initialize(variable, ...) \
  (MMK_EXPAND(MMK_APPLY_N_(MMK_COMMA_APPEND, MMK_VA_NARGS(__VA_ARGS__), variable, __VA_ARGS__)) \
   variable)

#  define mmk_struct_literal(type, ...) \
  mmk_struct_initialize((mmk_literal<type, __COUNTER__>::storage), __VA_ARGS__)

#  define mmk_literal(type, value) \
  (mmk_literal<type, __COUNTER__>::storage = value)

# else /* !defined __cplusplus */

#  define mmk_literal(type, value) ((type) value)

#  define mmk_struct_literal(type, ...) ((type) { __VA_ARGS__ })

# endif

#endif /* !MMK_LITERAL_H_ */
