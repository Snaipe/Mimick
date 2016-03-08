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
#if defined __linux__
# define _GNU_SOURCE
#endif

#include <string.h>
#include "assert.h"
#include "trampoline.h"
#define PAGE_SIZE 0x1000

extern void mmk_trampoline();
extern void mmk_trampoline_end();

#if defined __linux__
# include <unistd.h>
# include <sys/mman.h>

plt_fn *create_trampoline (void *ctx, plt_fn *routine)
{
    uintptr_t trampoline_sz = (uintptr_t) mmk_trampoline_end
                            - (uintptr_t) mmk_trampoline;

    mmk_assert (trampoline_sz < PAGE_SIZE);
    void **map = mmap (NULL, PAGE_SIZE,
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);

    *map = ctx;
    *(map + 1) = (void *) routine;
    memcpy (map + 2, mmk_trampoline, trampoline_sz);
    mprotect (map, PAGE_SIZE, PROT_READ | PROT_EXEC);
    return (plt_fn *) (map + 2);
}

void destroy_trampoline (plt_fn *trampoline)
{
    munmap ((void **) trampoline - 2, PAGE_SIZE);
}
#elif defined _WIN32
# include <windows.h>
# include "assert.h"

plt_fn *create_trampoline (void *ctx, plt_fn *routine)
{
    uintptr_t trampoline_sz = (uintptr_t) mmk_trampoline_end
                            - (uintptr_t) mmk_trampoline;

    mmk_assert (trampoline_sz < PAGE_SIZE);
    void **map = VirtualAlloc (NULL, PAGE_SIZE,
            MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    *map = ctx;
    *(map + 1) = (void *) routine;
    memcpy (map + 2, mmk_trampoline, trampoline_sz);

    DWORD old;
    VirtualProtect (map, PAGE_SIZE, PAGE_EXECUTE_READ, &old);
    return (plt_fn *) (map + 2);
}

void destroy_trampoline (plt_fn *trampoline)
{
    VirtualFree ((void **) trampoline - 2, 0, MEM_RELEASE);
}
#else
# error Unsupported platform
#endif
