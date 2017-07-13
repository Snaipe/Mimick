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

#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mimick/assert.h"

#include "plt.h"
#include "vitals.h"

#if MMK_BITS == 32
typedef struct mach_header mach_hdr;
typedef struct nlist sym;
typedef struct section section;
typedef struct segment_command segment_cmd;
#define MMK_LC_SEGMENT LC_SEGMENT
#elif MMK_BITS == 64
typedef struct mach_header_64 mach_hdr;
typedef struct nlist_64 sym;
typedef struct section_64 section;
typedef struct segment_command_64 segment_cmd;
#define MMK_LC_SEGMENT LC_SEGMENT_64
#else
# error Unsupported architecture
#endif

plt_ctx plt_init_ctx(void)
{
    return NULL;
}

plt_lib plt_get_lib(plt_ctx ctx, const char *name)
{
    (void) ctx;
    if (!name || !strcmp(name, "self"))
        return -1;

    const char *val = NULL;
    enum plt_selector sel = plt_get_selector(name, &val);
    size_t val_len = strlen(val);

    /* TODO: this is not thread safe, as another thread can load or unload
     * images on the fly -- find a way to fix this. */

    size_t nb_images = _dyld_image_count();
    for (size_t i = 1; i < nb_images; ++i) {
        if (sel == PLT_SEL_LIB) {
            size_t len = val_len + 8;
            char pattern[len];
            snprintf(pattern, len, "/lib%s.dylib", val);
            const char *img_name = _dyld_get_image_name(i);
            if (img_name && strstr(img_name, pattern))
                return i;
        } else if (sel == PLT_SEL_NONE || sel == PLT_SEL_FILE) {
            const char *img_name = _dyld_get_image_name(i);
            if (img_name && !strcmp(img_name, name))
                return i;
        } else if (sel == PLT_SEL_SYM) {
            plt_offset *off = plt_get_offsets(ctx, i, val, NULL);
            mmk_free(off);
            if (off)
                return i;
        }
    }
    return 0;
}

static size_t get_offsets(size_t *off, size_t n,
        const sym *symtab, const uint32_t *isymtab, size_t nsyms,
        const char *strtab, const char *name)
{
    size_t j = 0;
    for (size_t i = 0; j < n && i < nsyms; ++i) {
        if (isymtab[i] == INDIRECT_SYMBOL_LOCAL ||
            isymtab[i] == INDIRECT_SYMBOL_ABS)
            continue;

        const sym *s = &symtab[isymtab[i]];
        const char *symname = strtab + s->n_un.n_strx;

        if (!strcmp(symname + 1, name))
            off[j++] = i;
    }
    return j;
}

static inline void *ptr_add(const void *ptr, size_t off)
{
    return (char *) ptr + off;
}

static void find_tables(const struct mach_header *hdr,
        const sym **symtab, const char **strtab, const uint32_t **isymtab, size_t *nisyms)
{
    int symtab_found = 0, dysymtab_found = 0;
    const struct load_command *lc = ptr_add(hdr, sizeof (mach_hdr));
    for (size_t i = 0; i < hdr->ncmds; ++i, lc = ptr_add(lc, lc->cmdsize)) {
        if (lc->cmd == LC_SYMTAB) {
            const struct symtab_command *sc = (void *) lc;
            *symtab = ptr_add(hdr, sc->symoff);
            *strtab = ptr_add(hdr, sc->stroff);
            symtab_found = 1;
        } else if (lc->cmd == LC_DYSYMTAB) {
            const struct dysymtab_command *dsc = (void *) lc;
            *isymtab = ptr_add(hdr, dsc->indirectsymoff);
            *nisyms = dsc->nindirectsyms;
            dysymtab_found = 1;
        }

        if (symtab_found && dysymtab_found)
            return;
    }
}

static const section *get_section(const struct mach_header *hdr,
        const char *segname, const char *sectname)
{
    const struct load_command *lc = ptr_add(hdr, sizeof (mach_hdr));
    for (size_t i = 0; i < hdr->ncmds; ++i, lc = ptr_add(lc, lc->cmdsize)) {
        if (lc->cmd == MMK_LC_SEGMENT) {
            const segment_cmd *sc = (void *) lc;
            if (strncmp(segname, sc->segname, 16))
                continue;
            const section *s = ptr_add(sc, sizeof (segment_cmd));
            for (size_t j = 0; j < sc->nsects; ++j, ++s) {
                if (!strncmp(sectname, s->sectname, 16))
                    return s;
            }
        }
    }
    return NULL;
}

static inline plt_fn **find_offset_in(const struct mach_header *hdr,
        const section *sec, size_t *indices, size_t n)
{
    plt_fn **got = ptr_add(hdr, sec->offset);

    uint32_t idx_start = sec->reserved1;
    uint32_t count = sec->size / sizeof (void *);

    for (size_t i = 0; i < n; ++i) {
        if (indices[i] >= idx_start && indices[i] < idx_start + count)
            return &got[indices[i] - idx_start];
    }
    return NULL;
}

plt_offset *plt_get_offsets(plt_ctx ctx, plt_lib lib, const char *name, size_t *count)
{
    mmk_assert(lib);
    if (lib == -1)
        lib = 0;

    const struct mach_header *hdr = _dyld_get_image_header(lib);

    if (!hdr)
        return NULL;

    volatile const char *libname = _dyld_get_image_name(lib);
    (void) libname;

    const sym *symtab = NULL;
    const char *strtab = NULL;
    const uint32_t *isymtab = NULL;
    size_t nsyms = 0;

    find_tables(hdr, &symtab, &strtab, &isymtab, &nsyms);

    if (!symtab || !strtab || !isymtab || !nsyms)
        return NULL;

    size_t indices[32];

    size_t n = get_offsets(indices, 32, symtab, isymtab, nsyms, strtab, name);

    if (n == 0)
        return NULL;

    if (!count)
        return mmk_malloc(1);

    /* First offset is duplicated, so we ignore it by decrementing the count */
    plt_offset *ot = mmk_malloc(sizeof (plt_offset) * (n - 1));

    static const char *gots[] = {
        "__la_symbol_ptr", "__nl_symbol_ptr", "__got"
    };

    size_t offidx = 0;
    for (size_t sidx = 0; sidx < sizeof (gots) / sizeof (char *); ++sidx) {
        const section *got = get_section(hdr, SEG_DATA, gots[sidx]);
        if (!got)
            continue;

        plt_fn **off = find_offset_in(hdr, got, indices, n);
        if (!off)
            continue;

        ot[offidx++] = (plt_offset) { .offset = off };
    }
    *count = offidx;

    return ot;
}

void plt_set_offsets(plt_offset *offset, size_t nb_off, plt_fn *newval)
{
    for (size_t i = 0; i < nb_off; ++i) {
        if (!offset[i].oldval)
            offset[i].oldval = *offset[i].offset;
        *offset[i].offset = newval;
    }
}

void plt_reset_offsets(plt_offset *offset, size_t nb_off)
{
    for (size_t i = 0; i < nb_off; ++i) {
        *offset[i].offset = offset[i].oldval;
    }
}

plt_fn *plt_get_real_fn(plt_ctx ctx, const char *name)
{
    (void) ctx;

    /* We could iterate through the images ourselves, but dlsym is more
     * convenient here */
    return (plt_fn *) dlsym(RTLD_DEFAULT, name);
}
