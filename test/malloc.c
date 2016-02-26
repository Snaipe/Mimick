#include <stdlib.h>
#include <assert.h>
#include "mimick.h"

#define JUNK_PTR ((void *) 0x4242424242424242L)

void *malloc_mock (size_t size)
{
    return JUNK_PTR;
}

int main (void)
{
    mmk_init ();

    assert (malloc (4) != JUNK_PTR);

    mmk_mock m = mmk_mock_create ("malloc", NULL, (mmk_fn) malloc_mock);

    assert (malloc (4) == JUNK_PTR);

    mmk_mock_destroy (m);

    assert (malloc (4) != JUNK_PTR);

    return 0;
}
