#include <stdio.h>
#include <stdlib.h> 
#include <assert.h>
#include "mimick.h"

mmk_mock_define (realloc_mock, void *, void *, ptr, size_t, size);

int main(void)
{
    mmk_init ();

    mmk_mock m = mmk_mock_create ("realloc", NULL, realloc_mock);
    mmk_expect (m, realloc_mock, .size = 42, .ptr = NULL);
    mmk_expect (m, realloc_mock, .times = 3, .returning = (void*) 0x42);
    mmk_expect (m, realloc_mock, .times = 0, .returning = (void*) 0x43);
    mmk_expect (m, realloc_mock, .times = 2, .returning = (void*) 0x44);
    mmk_expect (m, realloc_mock, .times = -1, .returning = (void*) 0x10);

    for (int i = 0; i < 10; ++i)
        printf("%p\n", realloc(NULL, 42));
    mmk_mock_destroy (m);
    return 0;
}
