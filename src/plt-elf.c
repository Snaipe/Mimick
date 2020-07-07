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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mimick/assert.h"

#include "config.h"
#include "plt.h"
#include "trampoline.h"
#include "vitals.h"

#include <sys/mman.h>

#if MMK_BITS == 32
typedef Elf32_Word ElfWord;
typedef Elf32_Sword ElfSWord;
# ifndef ELF_R_SYM
#  define ELF_R_SYM(i) ELF32_R_SYM(i)
# endif
#elif MMK_BITS == 64
typedef Elf64_Xword ElfWord;
typedef Elf64_Sxword ElfSWord;
# ifndef ELF_R_SYM
#  define ELF_R_SYM(i) ELF64_R_SYM(i)
# endif
#else
# error Unsupported architecture
#endif

#if defined HAVE_ELF_AUXV_T
typedef ElfW(auxv_t) ElfAux;
#elif defined HAVE_ELF_AUXINFO
typedef ElfW(Auxinfo) ElfAux;
#else
# error Unsupported platform
#endif

# if defined __clang__
void __clear_cache(void *, void *);
# endif

extern char **environ;

static size_t get_offsets(plt_ctx ctx, plt_lib lib, const char *name, plt_offset ** offsets);

static void *lib_dt_lookup(plt_lib lib, ElfSWord tag)
{
    ElfW(Addr) base =(ElfW(Addr)) lib->l_addr;
    for (const ElfW(Dyn) *dyn = lib->l_ld; dyn->d_tag != DT_NULL; ++dyn) {
        if (dyn->d_tag == tag) {
            if (dyn->d_un.d_ptr >= base
                    && (dyn->d_un.d_ptr >> (MMK_BITS - 8)) ^ 0xff)
                return (void*) dyn->d_un.d_ptr;
            else
                return (char*) base + dyn->d_un.d_ptr;
        }
    }
    return NULL;
}

static ElfWord lib_dt_lookup_val(plt_lib lib, ElfSWord tag)
{
    for (const ElfW(Dyn) *dyn = lib->l_ld; dyn->d_tag != DT_NULL; ++dyn) {
        if (dyn->d_tag == tag) {
            return dyn->d_un.d_val;
        }
    }
    return 0;
}

#if !defined HAVE__R_DEBUG
static int find_dynamic(struct dl_phdr_info *info, size_t size, void *data)
{
    ElfAddr *ctx = data;

    for (ElfOff i = 0; i < info->dlpi_phnum; ++i) {
        if (info->dlpi_phdr[i].p_type == PT_DYNAMIC) {
            *ctx = info->dlpi_addr + info->dlpi_phdr[i].p_vaddr;
            return 1;
        }
    }
    return -1;
}

static struct r_debug *r_debug_from_dynamic(void *dynamic)
{
    for (const ElfW(Dyn) *dyn = dynamic; dyn->d_tag != DT_NULL; ++dyn) {
        if (dyn->d_tag == DT_DEBUG)
            return (struct r_debug *) dyn->d_un.d_ptr;
    }
    return NULL;
}
#endif

static struct r_debug *get_r_debug(void)
{
    // Find our own r_debug
    struct r_debug *dbg = NULL;

    // First use some known shortcuts
#if defined HAVE__R_DEBUG
    dbg = &_r_debug;
#elif defined HAVE__DYNAMIC
    dbg = r_debug_from_dynamic(_DYNAMIC);
#endif

#if !defined HAVE__R_DEBUG
    // If there are no available shortcuts, we manually query our own phdrs
# if defined HAVE__DYNAMIC
    if (!dbg) {
# endif
        ElfAddr dynamic;
        if (dl_iterate_phdr(find_dynamic, &dynamic) > 0)
            dbg = r_debug_from_dynamic((void *) dynamic);
# if defined HAVE__DYNAMIC
    }
# endif
#endif

    return dbg;
}

plt_ctx plt_init_ctx(void)
{
    static struct r_debug *dbg = (void*) -1;
    if (dbg == (void*) -1)
        dbg = get_r_debug();
    return dbg;
}

static const char *get_lib_name(plt_ctx ctx, plt_lib lib)
{
    /* The name of the main shared object is the empty string,
       we return something to be consistent with the eglibc weirdity */
    if (lib == ctx->r_map)
        return "self";

    /* Somewhy, eglibc always set l_name to the empty string. */
    if (lib->l_name[0])
        return lib->l_name;

    const char *strtab = lib_dt_lookup(lib, DT_STRTAB);
    ElfWord soname_off = lib_dt_lookup_val(lib, DT_SONAME);
    if (!strtab || soname_off == (ElfWord) - 1)
        return NULL;

    return &strtab[soname_off];
}

plt_lib plt_get_lib(plt_ctx ctx, const char *name)
{
    if (!name)
        return ctx->r_map;

    const char *val = NULL;
    enum plt_selector sel = plt_get_selector(name, &val);
    size_t val_len = strlen(val);
    int libc = !strcmp(val, "c");

    for (struct link_map *lm = ctx->r_map; lm != NULL; lm = lm->l_next) {
        if (sel == PLT_SEL_LIB) {
            const char *libname = get_lib_name(ctx, lm);
            if (libc) {
                if (strstr(libname, "/libc.so")
                 || strstr(libname, "/musl.so"))
                    return lm;
            } else {
                size_t len = val_len + 8;
                char pattern[len];
                snprintf(pattern, len, "/lib%s.so", val);
                if (strstr(libname, pattern))
                    return lm;
            }
        } else if (sel == PLT_SEL_NONE || sel == PLT_SEL_FILE) {
            const char *libname = get_lib_name(ctx, lm);
            if (!strcmp(name, libname))
                return lm;
        } else if (sel == PLT_SEL_SYM) {
            if (get_offsets(ctx, lm, val, NULL) > 0)
                return lm;
        }
    }
    return NULL;
}

struct rel_info {
    ElfW(Rel) *tab;
    ElfWord size;
    ElfWord entry_sz;
};

static uintptr_t get_offset(struct rel_info *info, ElfW(Sym) *symtab,
        const char *strtab, const char *name)
{
    ElfW(Rel) *rel = info->tab;
    for (ElfWord i = 0; i < info->size / info->entry_sz;
            ++i, rel = (void*)(((char *) rel) + info->entry_sz))
    {
        ElfW(Sym) *sym = &symtab[ELF_R_SYM(rel->r_info)];

        if (!strcmp(strtab + sym->st_name, name))
            return (uintptr_t) rel->r_offset;
    }
    return 0;
}

static size_t get_offsets(plt_ctx ctx, plt_lib lib, const char *name, plt_offset ** offsets)
{
    ElfW(Sym) *symtab   = (ElfW(Sym)*)  lib_dt_lookup(lib, DT_SYMTAB);
    const char *strtab  = (const char*) lib_dt_lookup(lib, DT_STRTAB);

    ElfW(Rel)   *jmprel = lib_dt_lookup(lib, DT_JMPREL);
    ElfW(Rel)   *rel    = lib_dt_lookup(lib, DT_RELA);
    ElfWord rel_sz = lib_dt_lookup_val(lib, DT_RELASZ);
    ElfWord jmprel_sz = lib_dt_lookup_val(lib, DT_PLTRELSZ);
    ElfWord relent_sz = lib_dt_lookup_val(lib, DT_RELAENT);

    if (!symtab || !strtab || !(rel || jmprel) || !(rel_sz || jmprel_sz) || !relent_sz)
        return 0;

    ElfW(Addr) base = (ElfW(Addr)) lib->l_addr;
#ifdef __FreeBSD__
    if (lib == ctx->r_map)
        base = 0;
#endif

    size_t n = 0;
    if (offsets) {
        *offsets = NULL;
    }

    struct rel_info info[] = {
        {
          .tab = rel,
          .size = rel_sz,
          .entry_sz = relent_sz,
        },
        {
          .tab = jmprel,
          .size = jmprel_sz,
          .entry_sz = relent_sz,
        }
    };

    for (size_t i = 0; i < sizeof(info) / sizeof(struct rel_info); ++i) {
        uintptr_t off = get_offset(&info[i], symtab, strtab, name);
        if (off) {
            if (offsets) {
                *offsets = mmk_realloc(*offsets, (n + 1) * sizeof(plt_offset));
                (*offsets)[n] = (plt_offset) { .offset = (plt_fn **)(base + off) };
            }
            ++n;
        }
    }

    return n;
}

plt_offset *plt_get_offsets(plt_ctx ctx, plt_lib lib, const char *name, size_t *n)
{
    plt_offset *ot = NULL;
    *n = get_offsets(ctx, lib, name, &ot);
    return ot;
}

#define align2_down(v, d) ((v) & ~((d) - 1))

void plt_set_offsets(plt_offset *offset, size_t nb_off, plt_fn *newval)
{
    for (size_t i = 0; i < nb_off; ++i) {
        if (!offset[i].oldval)
            offset[i].oldval = *offset[i].offset;
        void *page_start = (void *)align2_down((uintptr_t)offset[i].offset, 4096);

        /* making a page in a rx segment rwx is usually very bad practice,
           but this is a test context, we don't have to care as much.
           Implementing this right assumes that we have a way to know
           the protection of an existing page, which is not necessarily
           available on all unices. */
        mmk_mprotect(page_start, 4096, PROT_READ|PROT_WRITE|PROT_EXEC);
# if defined __clang__  // Check for Clang first, it may set __GNUC__ too.
        __clear_cache(page_start, page_start + 4096);
# elif defined __GNUC__
        __builtin___clear_cache((char *)page_start, (char *)(page_start + 4096));
# endif
        *offset[i].offset = newval;
    }
}

void plt_reset_offsets(plt_offset *offset, size_t nb_off)
{
    for (size_t i = 0; i < nb_off; ++i) {
        *offset[i].offset = offset[i].oldval;
    }
}

static unsigned long elf_hash (const char *s)
{
    unsigned long h = 0, high;
    while (*s) {
        h = (h << 4) + (unsigned char) *s++;
        if ((high = h & 0xf0000000))
            h ^= high >> 24;
        h &= ~high;
    }
    return h;
}

static ElfW(Sym) *elf_hash_find(ElfW(Word) *hash, ElfW(Sym) *symtab,
    const char *strtab, const char *name)
{
    struct {
        ElfW(Word) nb_buckets;
        ElfW(Word) nb_chains;
    } *h_info = (void*) hash;

    ElfW(Word) *buckets = (ElfW(Word)*) (h_info + 1);
    ElfW(Word) *chains = (ElfW(Word)*) (h_info + 1) + h_info->nb_buckets;

    unsigned long idx = elf_hash(name) % h_info->nb_buckets;

    for (ElfW(Word) si = buckets[idx]; si != STN_UNDEF; si = chains[si]) {
        if (mmk_streq(&strtab[symtab[si].st_name], name))
            return &symtab[si];
    }
    return NULL;
}

static ElfW(Sym) *sym_lookup_dyn(plt_lib lib, const char *name)
{
    ElfW(Word) *hash    = (ElfW(Word)*) lib_dt_lookup(lib, DT_HASH);
    ElfW(Sym) *symtab   = (ElfW(Sym)*)  lib_dt_lookup(lib, DT_SYMTAB);
    const char *strtab  = (const char*) lib_dt_lookup(lib, DT_STRTAB);

    if (!hash || !symtab || !strtab)
        return NULL;
    return elf_hash_find (hash, symtab, strtab, name);
}

plt_fn *plt_get_real_fn(plt_ctx ctx, const char *name)
{
    for (struct link_map *lm = ctx->r_map; lm != NULL; lm = lm->l_next) {
        ElfW(Sym) *sym = sym_lookup_dyn(lm, name);
        if (sym) {
            /* Some compilers (e.g. ICC) put unresolved symbols into the
               symbol table with a size of 0. We ignore them to avoid
               getting the address of the PLT stub. */
            if (sym->st_size == 0)
                continue;

            return (void *) (sym->st_value + lm->l_addr);
        }
    }
    return NULL;
}
