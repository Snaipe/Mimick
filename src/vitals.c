#include "assert.h"
#include "vitals.h"

void *(*mmk_malloc)(size_t);
void *(*mmk_realloc)(void *, size_t);
void (*mmk_free)(void *);

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

int mmk_isspace(int c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void mmk_init_vital_functions (plt_ctx ctx)
{
    mmk_assert (mmk_malloc  = (void *) plt_get_real_fn(ctx, "malloc"));
    mmk_assert (mmk_realloc = (void *) plt_get_real_fn(ctx, "realloc"));
    mmk_assert (mmk_free    = (void *) plt_get_real_fn(ctx, "free"));
}
