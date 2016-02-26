#if defined __linux__
# define _GNU_SOURCE
#endif

#include <string.h>
#include "trampoline.h"

extern void mmk_trampoline();
extern void mmk_trampoline_end();

#if defined __linux__
# include <unistd.h>
# include <sys/mman.h>

# define PAGE_SIZE 0x400

plt_fn *create_trampoline (void *ctx, plt_fn *routine)
{
    uintptr_t trampoline_sz = (uintptr_t) mmk_trampoline_end
                            - (uintptr_t) mmk_trampoline;

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
#else
# error Unsupported platform
#endif
