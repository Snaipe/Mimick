#include <stdlib.h>
#include "libfoo.h"

void fn_vv(void)
{
    abort();
}

void fn_vi(int i)
{
    (void) i;
    abort();
}

int fn_iv(void)
{
    abort();
    return 0;
}

int fn_ii(int i)
{
    abort();
    return i;
}

void fn_vli(long l, int i)
{
    (void) l;
    (void) i;
    abort();
}

int fn_ili(long l, int i)
{
    (void) l;
    abort();
    return i;
}

void fn_vi_va(int i, ...)
{
    (void) i;
    abort();
}

int fn_ii_va(int i, ...)
{
    (void) i;
    abort();
}
