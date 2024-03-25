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

void fn_vf(float f)
{
    (void)f;
    abort();
    return; 
}

float fn_ff(float f)
{
    (void)f;
    abort();
    return f;
}

void fn_vd(double d)
{
    (void)d;
    abort();
    return;
}

double fn_dd(double d)
{
    abort();
    return d;
}
