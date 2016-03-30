Mimick
======

[![Unix Build Status](https://travis-ci.org/Snaipe/Mimick.svg?branch=master)](https://travis-ci.org/Snaipe/Mimick) 
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/Snaipe/Mimick?svg=true&branch=master)](https://ci.appveyor.com/project/Snaipe/Mimick/branch/bleeding)
[![License](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](https://github.com/Snaipe/Mimick/blob/master/LICENSE) 
[![Version](https://img.shields.io/badge/version-v0.1-orange.svg?style=flat)](https://github.com/Snaipe/Mimick/releases) 

A KISS, cross-platform C Mocking library

## Philosophy

**Warning**: this library is still experimental. Use at your own risk.

Mimick aims to be a simple of use and powerful mocking and stubbing library for C.

It doesn't rely on external code generation or compiler plugin to work -- simply
link the library to your tests, and you're good to go!

Mimick works by poisoning the GOT of the target dynamic module at runtime, making
any call to the target functions routed to one of its own stubs. The mock API
is only sugar on top of the stub API, and provides a default stub suitable for
testing interactions and specifying behaviour.

## Samples

Here is a simple usage of Mimick to mock the `malloc` function:

```c
#include <stdlib.h>
#include <assert.h>
#include <mimick.h>

/* Define the blueprint of a mock identified by `malloc_proto`
   that returns a `void *` and takes a `size_t size` parameter. */
mmk_mock_define (malloc_mock, void *, size_t, size);

int main(void) {
    /* Mock the malloc function in the current module using 
       the `malloc_mock` blueprint. */
    mmk_mock("malloc@self", malloc_mock);

    /* Tell the mock to return NULL and set errno to ENOMEM
       whatever the given parameter is. */
    void *result = NULL;
    mmk_when(malloc(mmk_any(size_t)),
            .then_return = &result,
            .then_errno = ENOMEM);

    assert(malloc(42) == result && errno == ENOMEM);

    mmk_reset(malloc);
}
```

Or, alternatively:

```c
malloc_mock mock = mmk_mock("malloc@self", malloc_mock);

void *result = NULL;
mmk_when(mock(mmk_any(size_t)),
        .then_return = &result,
        .then_errno = ENOMEM);

assert(malloc(42) == result && errno == ENOMEM);

mmk_reset(mock);
```

Other samples may be found in the [samples](./sample/) directory.

## Compatibility matrix

Supported Compilers: GCC 4.6+, Clang 3.5+, MSVC 14+

Labels: **N/T**: Not Tested, **N/A**: Not Applicable

| Arch | Linux | OS X | FreeBSD | Windows |
| --- | --- | --- | --- | --- |
| x86 | ✓ | ✓ | ✓ | ✓ |
| x86\_64 | ✓ | ✓ | ✓ | ✓ |
| PPC | ✕ | ✕ | ✕ | N/A |

## F.A.Q.

**Q. Can I mock/stub static functions?**  
A. No. Static functions are, by definition, *private* implementations details,
   and because they are not exported nor visible to the outside world they cannot
   (and should not) be mocked/stubbed.

**Q. Can I mock/stub functions inside a static library?**  
A. *Maybe*. Functions inside a static library are moved inside the executable and are not
   called using the PLT by default. You need to build your library to use Position-Independent
   Code, otherwise the functions cannot be stubbed nor mocked.

**Q. I am mocking a standard library function, but `mmk_when` and `mmk_verify`
  are not working, why is this happening?**  
A. It's very possible that your compiler is optimizing away your function
   call inside `mmk_when` or `mmk_verify` since it has determined that there
   are no visible side effects in not calling the function. Compile your tests
   without optimizations (with a compiler flag or the `mmk_no_optimize`
   function attribute if it is available to your compiler), or use the function
   pointer returned by `mmk_mock` rather than the function itself.

**Q. Does this run on embedded systems / bare metal?**  
A. No. Unless your target system supports dynamic linking, Mimick won't work *at all*.
