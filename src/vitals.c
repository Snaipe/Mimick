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
#include <stdarg.h>

#include "mimick/assert.h"
#include "mimick/preprocess.h"

#include "vitals.h"

int mmk_memcmp(const void *s1, const void *s2, size_t n)
{
    for (const char *s1_ = s1, *s2_ = s2; n; ++s1_, ++s2_, --n)
        if (*s1_ != *s2_)
            return *s1_ < *s2_ ? -1 : 1;
    return 0;
}

void *mmk_memcpy(void *dst, const void *src, size_t n)
{
    const char *src_ = src;
    for (char *dst_ = dst; n; ++dst_, ++src_, --n)
        *dst_ = *src_;
    return dst;
}

int mmk_strneq(const char *src, const char *ref, size_t n)
{
    for (; *src && *ref && n; ++src, ++ref, --n) {
        if (*src != *ref)
            return 0;
    }
    return n == 0 || !*ref;
}

char *mmk_strchr(const char *buf, int c)
{
    for (; *buf; ++buf)
        if (*buf == c)
            return (char *) buf;
    return NULL;
}

char *mmk_strcpy(char *dst, const char *src)
{
    for (; *src; ++dst, ++src)
        *dst = *src;
    *dst = '\0';
    return dst;
}

char *mmk_strncpy(char *dst, const char *src, size_t n)
{
    for (; *src && n; ++dst, ++src, --n)
        *dst = *src;
    *dst = '\0';
    return dst;
}

size_t mmk_strlen(const char *s)
{
    size_t len = 0;
    for (; *s; ++len, ++s);
    return len;
}

int mmk_isspace(int c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void *(*mmk_malloc_)(size_t);
void *mmk_malloc(size_t size)
{
    return mmk_malloc_(size);
}

void *(*mmk_realloc_)(void *, size_t);
void *mmk_realloc(void *ptr, size_t size)
{
    return mmk_realloc_(ptr, size);
}

void (*mmk_free_)(void *);
void mmk_free(void *ptr)
{
    mmk_free_(ptr);
}

void (*mmk_abort_)(void);
void mmk_abort(void)
{
    mmk_abort_();
}

void (*mmk_vfprintf_)(FILE *, const char *, va_list);
void mmk_fprintf(FILE *f, const char *str, ...)
{
    va_list vl;
    va_start(vl, str);
    mmk_vfprintf_(f, str, vl);
    va_end(vl);
}

/* Never called, this is used to put vital functions into our own PLT for
   fast lookups */
void mmk_fetch_plt(void)
{
    va_list vl;
    vfprintf(stderr, "%s", vl);
    abort();
}

void mmk_init_vital_functions(plt_ctx ctx)
{
    /* We initialize the GOT entries for these functions by calling them
       beforehand */
    free(realloc(malloc(0), 0));

    mmk_assert(mmk_malloc_      = (void *) plt_get_real_fn(ctx, "malloc"));
    mmk_assert(mmk_realloc_     = (void *) plt_get_real_fn(ctx, "realloc"));
    mmk_assert(mmk_free_        = (void *) plt_get_real_fn(ctx, "free"));
    mmk_assert(mmk_vfprintf_    = (void *) plt_get_real_fn(ctx, "vfprintf"));
    mmk_assert(mmk_abort_       = (void *) plt_get_real_fn(ctx, "abort"));
}
