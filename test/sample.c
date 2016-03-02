#undef NDEBUG
#include <stdio.h>
#include <stdlib.h> 
#include <assert.h>
#include "mimick.h"

mmk_mock_define (realloc_proto, void *, void *, ptr, size_t, size);

int main(void)
{
    mmk_init ();

    mmk_mock mock = mmk_mock_create ("realloc", realloc_proto);
    mmk_expect (realloc_proto, mock, .size = 42, .ptr = NULL);
    mmk_expect (realloc_proto, mock, .times = 3, .returning = (void*) 0x42);
    mmk_expect (realloc_proto, mock, .times = 0, .returning = (void*) 0x43);
    mmk_expect (realloc_proto, mock, .times = 2, .returning = (void*) 0x44);
    mmk_expect (realloc_proto, mock, .times = -1, .returning = (void*) 0x10);

    for (int i = 0; i < 10; ++i)
        printf("%p\n", realloc(NULL, 42));
    mmk_mock_destroy (mock);
    return 0;
}
