#undef NDEBUG
#include <stdlib.h>
#include <assert.h>
#include "mimick.h"

#define JUNK_PTR ((void *) 0x4242424242424242L)

void *malloc_mock (size_t size)
{
    (void) size;
    return JUNK_PTR;
}

int main (void)
{
    mmk_init ();

    assert (malloc (4) != JUNK_PTR);

    mmk_mock m = mmk_mock_create ("malloc", NULL, (mmk_fn) malloc_mock);

    /* We need volatile here because GCC aggressively optimises the code
       to always fail, since it isn't expected for malloc to return a
       fixed value like JUNK_PTR */
    void *volatile ptr = malloc (4);
    assert (ptr == JUNK_PTR);

    mmk_mock_destroy (m);

    assert (malloc (4) != JUNK_PTR);

    return 0;
}
