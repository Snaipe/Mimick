#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <link.h>
#define HAVE__R_DEBUG 1

extern char **environ;

static void *dt_lookup(void *base, ElfW(Dyn) *dyn, ElfW(Sxword) tag)
{
    for (; dyn->d_tag != DT_NULL; ++dyn) {
        if (dyn->d_tag == tag) {
            if ((void *) dyn->d_un.d_ptr >= base
                    && ((ElfW(Sxword)) dyn->d_un.d_ptr) >= 0)
                return (void*) dyn->d_un.d_ptr;
            else
                return (char*) base + dyn->d_un.d_ptr;
        }
    }
    return NULL;
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

typedef struct link_map *plt_lib;
typedef struct r_debug *plt_ctx;
typedef void *plt_got;
typedef void (plt_fn)(void);

static inline void *lib_dt_lookup(plt_lib lib, ElfW(Sxword) tag)
{
    return dt_lookup((void*) lib->l_addr, lib->l_ld, tag);
}

plt_ctx plt_init_ctx(void)
{
    static struct r_debug *dbg = NULL;
    if (!dbg)
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

void *plt_get_got(plt_lib lib)
{
    return dt_lookup((void*) lib->l_addr, lib->l_ld, DT_PLTGOT);
}

struct rel_info {
    ElfW(Rel) *tab;
    ElfW(Xword) size;
    ElfW(Xword) entry_sz;
};

static uintptr_t get_offset(struct rel_info *info, ElfW(Sym) *symtab, const char *strtab, const char *name)
{
    ElfW(Rel) *rel = info->tab;
    for (ElfW(Xword) i = 0; i < info->size / info->entry_sz;
            ++i, rel = (void*)(((char *) rel) + info->entry_sz))
    {
        ElfW(Sym) *sym = &symtab[ELF64_R_SYM(rel->r_info)];

        if (!strcmp(strtab + sym->st_name, name))
            return (uintptr_t) rel->r_offset;
    }
    return 0;
}

plt_fn **plt_get_offset(plt_lib lib, const char *name)
{
    ElfW(Sym) *symtab   = (ElfW(Sym)*)  lib_dt_lookup(lib, DT_SYMTAB);
    const char *strtab  = (const char*) lib_dt_lookup(lib, DT_STRTAB);
    ElfW(Sxword) type = (ElfW(Sxword)) lib_dt_lookup(lib, DT_PLTREL);

    ElfW(Rel)   *rel = lib_dt_lookup(lib, DT_JMPREL);
    ElfW(Xword) rel_sz = (ElfW(Xword)) lib_dt_lookup(lib, DT_PLTRELSZ);
    ElfW(Xword) relent_sz = (ElfW(Xword)) lib_dt_lookup(lib, type + 2);

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
