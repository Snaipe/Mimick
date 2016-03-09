#define DLL_EXPORT 1
#include <string.h>
#include <stdlib.h>
#include "strdup.h"

char *strdup(const char *s)
{
    size_t len = strlen (s);
    char *dst = malloc (len + 1);
    if (!dst)
        return NULL;
    memcpy (dst, s, len + 1);
    return dst;
}
