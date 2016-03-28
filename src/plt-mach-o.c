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

#include <mach-o/getsect.h>
#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>

#if MMK_BITS == 32
typedef struct mach_header mach_hdr;
typedef struct nlist sym;
#elif MMK_BITS == 64
typedef struct mach_header_64 mach_hdr;
typedef struct nlist_64 sym;
#else
# error Unsupported architecture
#endif

static inline void *get_real_address(plt_lib lib, void *addr) {
    if (!addr)
        return NULL;
    return (char *) addr + _dyld_get_image_vmaddr_slide(lib);
}

plt_ctx plt_init_ctx(void)
{
    return NULL;
}

plt_lib plt_get_lib(plt_ctx ctx, const char *name)
{
    (void) ctx;
    if (!name || !strncmp(name, "self"))
        return 0;

    enum selector {
        NONE, LIB, FILE, SYM
    };

    enum selector sel = NONE;
    if (!strncmp(name, "lib:", 4))
        sel = LIB;
    else if (!strncmp(name, "file:", 5))
        sel = FILE;
    else if (!strncmp(name, "sym:", 5))
        sel = SYM;
    else {

        char *end_sel = strchr(name, ':');
        if (end_sel) {
            size_t len = (size_t) (end_sel - name + 1);
            fprintf(stderr, "mimick: unknown '%.*s' selector.\n", (int) len, name);
        } else {
            fprintf(stderr, "mimick: unknown target kind '%s'.\n", name);
        }
        abort();
    }

    const char *val = sel == NONE ? name : strchr(name, ':') + 1;
    size_t val_len = strlen(val);

    /* TODO: this is not thread safe, as another thread can load or unload
     * images on the fly -- find a way to fix this. */

    size_t nb_images = _dyld_image_count();
    for (size_t i = 1; i < nb_images; ++i) {
        const struct mach_header *hdr = _dyld_get_image_header(i);

        if (sel == LIB) {
            size_t len = val_len + 8;
            char pattern[len];
            snprintf(pattern, len, "/lib%s.dylib", val);
            const char *img_name = _dyld_get_image_name(i);
            if (img_name && strstr(img_name, pattern))
                return i;
        } else if (sel == NONE || sel == FILE) {
            const char *img_name = _dyld_get_image_name(i);
            if (img_name && !strcmp(img_name, name))
                return i;
        } else if (sel == SYM) {
            plt_fn **sym = plt_get_offset(i, val);
            if (sym)
                return i;
        }
    }
}

static plt_fn **get_offset(plt_fn **got, const sym *symtab, size_t nsyms,
        const char *strtab, const char *name)
{
    for (size_t i = 0; i < nsyms; ++i) {
        const char *symname = strtab + symtab[i].n_un.n_strx;
        if (!strcmp(symname + 1, name))
            return &got[i];
    }
    return NULL;
}

static inline void *ptr_add(void *ptr, size_t off)
{
    return (char *) ptr + off;
}

static void find_tables(const struct mach_header *hdr,
        const sym **symtab, size_t *nsyms, const char **strtab)
{
    const struct load_command *lc = ptr_add(hdr, sizeof (mach_hdr));
    for (size_t i = 0; i < hdr->ncmds; ++i, lc = ptr_add(lc, lc->cmdsize)) {
        if (lc.cmd == LC_SYMTAB) {
            const struct symtab_command *sc = (void *) lc;
            *symtab = ptr_add(hdr, sc->symoff);
            *strtab = ptr_add(hdr, sc->stroff);
            *nsyms = sc->nsyms;
            return;
        }
    }
}

plt_fn **plt_get_offset(plt_lib lib, const char *name)
{
    size_t sz;
    const struct mach_header *hdr = _dyld_get_image_header(lib);

    if (!hdr)
        return NULL;

    plt_fn **got = getsectdatafromheader(hdr, "__DATA", "__la_symbol_ptr", &sz);
    got = get_real_address(lib, got);

    if (!got)
        return NULL;

    const sym *symtab = NULL;
    const char *strtab = NULL;
    size_t nsyms = 0;

    find_tables(hdr, &symtab, &nsyms, &strtab);

    return get_offset(got, symtab, nsyms, strtab, name);
}

void plt_set_offset(plt_fn **offset, plt_fn *newval)
{
    *offset = newval;
}

plt_fn *plt_get_real_fn(plt_ctx ctx, const char *name)
{
    /* TODO: same as above, this is not thread safe. */

    size_t nb_images = _dyld_image_count();
    for (size_t i = 1; i < nb_images; ++i) {
        plt_fn **sym = plt_get_offset(i, val);
        if (sym)
            return *sym;
    }
    return NULL;
}

#endif /* !PLT_H_ */
