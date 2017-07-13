#include <mimick.h>
#include <mimick/assert.h>
#include <errno.h>
#include <string.h>
#include "strdup.h"

mmk_mock_define(malloc_mock, void *, size_t);

void test_simple_case(void)
{
    static char ref[] = "hello";
    char buf[sizeof (ref)];

    malloc_mock m = mmk_mock("malloc@lib:strdup", malloc_mock);
    mmk_assert((mmk_fn) m != MMK_MOCK_INVALID);

    mmk_when(malloc(sizeof(ref)), .then_return = &(char*) { buf });

    char *dup = my_strdup("hello");

    /* mmk_verify is overkill in this case, mmk_verify should be used when you
     * care about testing interactions rather than external behaviour */
    int pass = mmk_verify(malloc(mmk_ge(size_t, sizeof (ref))), .times = 1);

    mmk_assert(pass);
    mmk_assert(dup == buf && !strcmp(ref, buf));

    mmk_reset(malloc);
}

void test_error_case(void)
{
    /* Alternative usage */
    malloc_mock mock = mmk_mock("malloc@lib:strdup", malloc_mock);
    mmk_assert((mmk_fn) mock != MMK_MOCK_INVALID);

    mmk_when(mock(mmk_any(size_t)),
            .then_return = &(void *) { NULL },
            .then_errno = ENOMEM);

    char *dup = my_strdup("foo");
    mmk_assert(dup == NULL && errno == ENOMEM);

    mmk_reset(mock);
}

int main(void)
{
    test_simple_case();
    test_error_case();
    return 0;
}
