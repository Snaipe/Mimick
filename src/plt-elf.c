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
#include <assert.h>
#include <string.h>
#include "plt-elf.h"
#include "trampoline.h"
#include "config.h"

#if MMK_BITS == 32
typedef ElfW(Word) ElfWord;
typedef ElfW(Sword) ElfSWord;
# define ELF_R_SYM(i) ELF32_R_SYM(i)
#elif MMK_BITS == 64
typedef ElfW(Xword) ElfWord;
typedef ElfW(Sxword) ElfSWord;
# define ELF_R_SYM(i) ELF64_R_SYM(i)
#else
# error Unsupported architecture
#endif

extern char **environ;

static void *lib_dt_lookup(plt_lib lib, ElfW(Sxword) tag)
{
    for (ElfW(Dyn) *dyn = lib->l_ld; dyn->d_tag != DT_NULL; ++dyn) {
        if (dyn->d_tag == tag) {
            if (dyn->d_un.d_ptr >= lib->l_addr
                    && ((ElfSWord) dyn->d_un.d_ptr) >= 0)
                return (void*) dyn->d_un.d_ptr;
            else
                return (char*) lib->l_addr + dyn->d_un.d_ptr;
        }
    }
    return NULL;
}

static ElfWord lib_dt_lookup_val(plt_lib lib, ElfW(Sxword) tag)
{
    for (ElfW(Dyn) *dyn = lib->l_ld; dyn->d_tag != DT_NULL; ++dyn) {
        if (dyn->d_tag == tag) {
            return dyn->d_un.d_val;
        }
    }
    return 0;
}

static ElfW(Addr) get_auxval(ElfW(auxv_t) *auxv, ElfW(Off) tag)
{
    for (; auxv->a_type != AT_NULL; auxv++) {
        if (auxv->a_type == tag)
            return auxv->a_un.a_val;
    }
    return (ElfW(Addr)) -1;
}

static ElfW(Addr) find_dynamic(ElfW(Phdr) *phdr, ElfW(Off) phent)
{
    for (ElfW(Off) i = 0; i < phent; ++i) {
        if (phdr[i].p_type == PT_DYNAMIC)
            return phdr[i].p_vaddr;
    }
    return 0;
}

static struct r_debug *r_debug_from_dynamic (void *dynamic)
{
    for (ElfW(Dyn) *dyn = dynamic; dyn->d_tag != DT_NULL; ++dyn) {
        if (dyn->d_tag == DT_DEBUG)
            return (struct r_debug *) dyn->d_un.d_ptr;
    }
    return NULL;
}

static struct r_debug *get_r_debug (void)
{
    // Find our own r_debug
    struct r_debug *dbg = NULL;

    // First use some known shortcuts
#if defined HAVE__R_DEBUG
    dbg = &_r_debug;
#elif defined HAVE__DYNAMIC
    dbg = r_debug_from_dynamic(_DYNAMIC);
#endif

    // If there are no available shortcuts, we manually query our own phdrs
    if (!dbg) {
        char **envp = environ;
        while (*envp++ != NULL);
        ElfW(auxv_t) *auxv = (ElfW(auxv_t)*) envp;
        ElfW(Addr) phdr = get_auxval(auxv, AT_PHDR);
        ElfW(Addr) phent = get_auxval(auxv, AT_PHENT);
        if (phdr != (ElfW(Addr)) -1 && phent != (ElfW(Addr)) -1) {
            ElfW(Addr) dynamic = find_dynamic((void*) phdr, phent);
            dbg = r_debug_from_dynamic((void*) dynamic);
        }
    }

    return dbg;
}

plt_ctx plt_init_ctx(void)
{
    static struct r_debug *dbg = (void*) -1;
    if (dbg == (void*) -1)
        dbg = get_r_debug();
    return dbg;
}

plt_lib plt_get_lib(plt_ctx ctx, const char *name)
{
    if (!name)
        name = "";

    for (struct link_map *lm = ctx->r_map; lm != NULL; lm = lm->l_next) {
        if (!strcmp(name, lm->l_name))
            return lm;
    }
    return NULL;
}

struct rel_info {
    ElfW(Rel) *tab;
    ElfW(Xword) size;
    ElfW(Xword) entry_sz;
};

static uintptr_t get_offset(struct rel_info *info, ElfW(Sym) *symtab,
        const char *strtab, const char *name)
{
    ElfW(Rel) *rel = info->tab;
    for (ElfW(Xword) i = 0; i < info->size / info->entry_sz;
            ++i, rel = (void*)(((char *) rel) + info->entry_sz))
    {
        ElfW(Sym) *sym = &symtab[ELF_R_SYM(rel->r_info)];

        if (!strcmp(strtab + sym->st_name, name))
            return (uintptr_t) rel->r_offset;
    }
    return 0;
}

plt_fn **plt_get_offset(plt_lib lib, const char *name)
{
    ElfW(Sym) *symtab   = (ElfW(Sym)*)  lib_dt_lookup(lib, DT_SYMTAB);
    const char *strtab  = (const char*) lib_dt_lookup(lib, DT_STRTAB);
    ElfSWord type = (ElfSWord) lib_dt_lookup_val(lib, DT_PLTREL);

    ElfW(Rel)   *rel = lib_dt_lookup(lib, DT_JMPREL);
    ElfWord rel_sz = lib_dt_lookup_val(lib, DT_PLTRELSZ);
    ElfWord relent_sz = lib_dt_lookup_val(lib, type + 2);

    if (!symtab || !strtab || !type || !rel || !rel_sz || !relent_sz)
        return NULL;

    struct rel_info info = {
        .tab = rel,
        .size = rel_sz,
        .entry_sz = relent_sz,
    };

    uintptr_t off = get_offset(&info, symtab, strtab, name);
    if (off)
        return (plt_fn **) (off + lib->l_addr);
    return NULL;
}

void plt_set_offset(plt_fn **offset, plt_fn *newval)
{
    *offset = newval;
}
