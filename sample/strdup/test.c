#include <mimick.h>
#include <errno.h>
#include "assert.h"
#include "strdup.h"

mmk_mock_define (malloc, void *, size_t, size);

void test_simple_case(void)
{
    static char ref[] = "hello";
    char buf[sizeof (ref)];

    mmk_mock m = mmk_mock_create("malloc@lib:strdup", malloc);
    mmk_when (m, malloc, with(sizeof (ref)), .then_return = buf);

    char *dup = strdup("hello");

    /* mmk_verify is overkill in this case, mmk_verify should be used when you
     * care about testing interactions rather than external behaviour */
    mmk_verify (m, malloc,
            .that_size = mmk_geq(size_t, sizeof (ref)),
            .times = 1);

    mmk_assert(dup == buf && !strcmp(ref, buf));

    mmk_mock_destroy (m);
}

void test_error_case(void)
{
    mmk_mock m = mmk_mock_create("malloc@lib:strdup", malloc);

    mmk_when (m, malloc, with(mmk_any(size_t)),
            .then_return = NULL,
            .then_errno = ENOMEM);

    char *dup = strdup("foo");
    mmk_assert(dup == NULL && errno == ENOMEM);

    mmk_mock_destroy (m);
}

int main(void)
{
    mmk_init();
    test_simple_case();
    test_error_case();
    return 0;
}
