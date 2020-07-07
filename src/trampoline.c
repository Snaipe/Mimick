/*
 * The MIT License (MIT)
 *
 * Copyright © 2016 Franklin "Snaipe" Mathieu <http://snai.pe/>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <string.h>

#include "mimick/assert.h"

#include "trampoline.h"
#include "vitals.h"

#define PAGE_SIZE 0x1000

extern void mmk_trampoline();
extern void mmk_trampoline_end();

#if defined HAVE_MMAP
# include <unistd.h>
# include <sys/mman.h>

# ifndef HAVE_MMAP_MAP_ANONYMOUS
#  include <fcntl.h>
# endif

# if defined __clang__
void __clear_cache(void *, void *);
# endif

plt_fn *create_trampoline(void *ctx, plt_fn *routine)
{
    uintptr_t trampoline_sz = (uintptr_t) mmk_trampoline_end
                            - (uintptr_t) mmk_trampoline;

    mmk_assert(trampoline_sz < PAGE_SIZE);

# if defined HAVE_MMAP_MAP_ANONYMOUS
    void **map = mmap(NULL, PAGE_SIZE,
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);
# elif defined HAVE_MMAP_MAP_ANON
    void **map = mmap(NULL, PAGE_SIZE,
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE | MAP_ANON,
            -1, 0);
# else
    int fd = open("/dev/zero", O_RDWR);
    mmk_assert(fd != -1);

    void **map = mmap(NULL, PAGE_SIZE,
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE,
            fd, 0);

    mmk_assert(close(fd) != -1);
# endif

    mmk_assert(map != MAP_FAILED);

    *map = ctx;
    *(map + 1) = (void *) routine;
    memcpy(map + 2, mmk_trampoline, trampoline_sz);
    mmk_assert(!mmk_mprotect(map, PAGE_SIZE, PROT_READ | PROT_EXEC));
# if defined __clang__  // Check for Clang first, it may set __GNUC__ too.
    __clear_cache(map, map + PAGE_SIZE);
# elif defined __GNUC__
    __builtin___clear_cache((char *)map, (char *)(map + PAGE_SIZE));
# endif
    return (plt_fn *) (map + 2);
}

void destroy_trampoline(plt_fn *trampoline)
{
    munmap((void **) trampoline - 2, PAGE_SIZE);
}
#elif defined _WIN32
# include <windows.h>

plt_fn *create_trampoline(void *ctx, plt_fn *routine)
{
    uintptr_t trampoline_sz = (uintptr_t) mmk_trampoline_end
                            - (uintptr_t) mmk_trampoline;

    mmk_assert(trampoline_sz < PAGE_SIZE);
    void **map = VirtualAlloc(NULL, PAGE_SIZE,
            MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    *map = ctx;
    *(map + 1) = (void *) routine;
    memcpy(map + 2, mmk_trampoline, trampoline_sz);

    DWORD old;
    VirtualProtect(map, PAGE_SIZE, PAGE_EXECUTE_READ, &old);
    return (plt_fn *) (map + 2);
}

void destroy_trampoline(plt_fn *trampoline)
{
    VirtualFree((void **) trampoline - 2, 0, MEM_RELEASE);
}
#else
# error Unsupported platform
#endif
