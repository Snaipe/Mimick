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

    void *ptr = realloc(NULL, 42);
    printf("%p\n", ptr);
    mmk_mock_destroy (m);
    return 0;
}
