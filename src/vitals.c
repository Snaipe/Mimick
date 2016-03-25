#include "assert.h"
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

void mmk_init_vital_functions(plt_ctx ctx)
{
    /* Fetch the following function into our own plt */
    free(realloc(malloc(0), 0));

    mmk_assert(mmk_malloc_  = (void *) plt_get_real_fn(ctx, "malloc"));
    mmk_assert(mmk_realloc_ = (void *) plt_get_real_fn(ctx, "realloc"));
    mmk_assert(mmk_free_    = (void *) plt_get_real_fn(ctx, "free"));
}
