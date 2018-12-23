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
#ifndef STRING_H_
# define STRING_H_

# include <stddef.h>
# include "mimick/unmocked.h"
# include "common.h"
# include "plt.h"

/* string functions that are notoriously replaced by sse2 variants are
 * reimplemented. */
int mmk_memcmp(const void *s1, const void *s2, size_t n);
void *mmk_memcpy(void *dst, const void *src, size_t n);
char *mmk_strchr(const char *s, int c);
int mmk_strneq(const char *src, const char *ref, size_t n);
int mmk_streq(const char *src, const char *ref);
char *mmk_strcpy(char *dst, const char *src);
char *mmk_strncpy(char *dst, const char *src, size_t n);
size_t mmk_strlen(const char *s);
int mmk_isspace(int c);
int mmk_mprotect(void *addr, size_t len, int prot);

mmk_noreturn void mmk_panic(const char *, ...);

void mmk_init_vital_functions(plt_ctx ctx);

#endif /* !STRING_H_ */
