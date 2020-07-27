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

#  include <cstdarg>
#  include <utility>

#  include "preprocess.h"

template<typename T, int ID>
struct mmk_literal {
  static constexpr int id = ID;
  static T storage;
};

template<typename T, int ID>
T mmk_literal<T, ID>::storage;

template <typename T>
T & mmk_assign(T & dst, T src) {
  return dst = std::move(dst);
}

template <typename T, size_t N>
T (& mmk_assign(T (&dst)[N], T * src))[N] {
  if (src) {
    mmk_memcpy(dst, src, sizeof(T) * N);
  } else {
    mmk_memset(dst, 0, sizeof(T) * N);
  }
  return dst;
}

va_list & mmk_assign(va_list & dst, va_list src) {
  if (src) {
    va_copy(dst, src);
  }
  return dst;
}

#  define MMK_COMMA_APPEND(_, prefix, content) (void) (prefix content),

#  define mmk_struct_initialize(variable, ...) \
  (MMK_EXPAND(MMK_APPLY_N_(MMK_COMMA_APPEND, MMK_VA_NARGS(__VA_ARGS__), variable, __VA_ARGS__)) \
   variable)

#  define mmk_struct_literal(type, ...) \
  mmk_struct_initialize((mmk_literal<type, __COUNTER__>::storage), __VA_ARGS__)

#  define mmk_literal(type, value) \
  (mmk_assign(mmk_literal<type, __COUNTER__>::storage, value))

# else /* !defined __cplusplus */

#  define mmk_assign(dst, src) (dst) = (src)

#  define mmk_literal(type, value) ((type) value)

#  define mmk_struct_literal(type, ...) ((type) { __VA_ARGS__ })

# endif

#endif /* !MMK_LITERAL_H_ */
