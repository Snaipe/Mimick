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
#include "plt.h"

#include <malloc.h>
#include <stdint.h>
#include <tlhelp32.h>

#include "mimick/assert.h"

#include "vitals.h"

#define IDIR_IMPORT 1 // Index of the import directory entry

static plt_fn **plt_get_offset(plt_lib lib, const char *name);

plt_ctx plt_init_ctx(void) {
    return NULL;
}

static plt_fn **plt_find_offset(const char *name, plt_lib *lib)
{
    DWORD errorMessageID;
    HANDLE snap;
    do {
        snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,
            GetCurrentProcessId());
        if (snap != INVALID_HANDLE_VALUE) {
            break;
        }
        errorMessageID = GetLastError();
    } while (errorMessageID == ERROR_BAD_LENGTH);

    mmk_assert(snap != INVALID_HANDLE_VALUE);

    MODULEENTRY32 mod = { .dwSize = sizeof(MODULEENTRY32) };
    for (BOOL more = Module32First(snap, &mod); more;
            more = Module32Next(snap, &mod))
    {
        plt_fn **fn = plt_get_offset(mod.hModule, name);
        if (fn) {
            if (lib)
                *lib = mod.hModule;
            return fn;
        }
    }
    mmk_assert(GetLastError() == ERROR_NO_MORE_FILES);
    return NULL;
}

plt_lib plt_get_lib(plt_ctx ctx, const char *name)
{
    (void) ctx;
    if (!name)
        name = "self";

    if (mmk_streq(name, "self"))
        return GetModuleHandle(NULL);

    const char *val = NULL;
    enum plt_selector sel = plt_get_selector(name, &val);
    size_t val_len = mmk_strlen(val);

    if (sel == PLT_SEL_FILE || sel == PLT_SEL_LIB) {
        HMODULE m = GetModuleHandle(val);
        if (!m) {
            size_t sz = val_len + 4;
            char *buf = alloca(sz);
            snprintf(buf, sz, "lib%s", val);
            m = GetModuleHandle(buf);
        }
        return m;
    } else if (sel == PLT_SEL_SYM) {
        plt_lib lib;
        plt_fn **fn = plt_find_offset(name + 4, &lib);
        if (fn)
            return lib;
    }
    return NULL;
}

static inline PIMAGE_NT_HEADERS nt_header_from_lib(plt_lib lib)
{
    PIMAGE_DOS_HEADER dos_hdr = (PIMAGE_DOS_HEADER) lib;
    return (PIMAGE_NT_HEADERS) ((char *) dos_hdr + dos_hdr->e_lfanew);
}

static inline PIMAGE_IMPORT_DESCRIPTOR get_first_import_descriptor(plt_lib lib)
{
    PIMAGE_NT_HEADERS nthdr = nt_header_from_lib(lib);
    DWORD off = nthdr->OptionalHeader.DataDirectory[IDIR_IMPORT].VirtualAddress;
    if (!off)
        return NULL;
    return (PIMAGE_IMPORT_DESCRIPTOR) ((char *) lib + off);
}

static plt_fn **plt_get_offset(plt_lib lib, const char *name)
{
    char *base = lib;
    for (PIMAGE_IMPORT_DESCRIPTOR entry = get_first_import_descriptor(lib);
            entry && entry->Name;
            entry++)
    {
        uintptr_t *thunk = (uintptr_t *) (base + entry->FirstThunk);
        PIMAGE_THUNK_DATA thunk_data = (PIMAGE_THUNK_DATA) (base
                + (entry->OriginalFirstThunk
                    ? entry->OriginalFirstThunk
                    : entry->FirstThunk));
        for (; thunk_data->u1.AddressOfData != 0; ++thunk, ++thunk_data)
        {
            PIMAGE_IMPORT_BY_NAME ibn = (void *) thunk_data->u1.AddressOfData;
            if (!strcmp(name, base + (uintptr_t) ibn->Name))
                return (plt_fn **) thunk;
        }
    }
    return NULL;
}

plt_offset *plt_get_offsets(plt_ctx ctx, plt_lib lib, const char *name, size_t *n)
{
    plt_fn **off = plt_get_offset(lib, name);
    if (off) {
        plt_offset *ot = mmk_malloc(sizeof (plt_offset));
        *n = 1;
        *ot = (plt_offset) { .offset = off };
        return ot;
    }
    return NULL;
}

static void plt_set_offset(plt_fn **offset, plt_fn *newval)
{
    DWORD old;
    VirtualProtect(offset, sizeof (void*), PAGE_EXECUTE_READWRITE, &old);
    *offset = newval;
    VirtualProtect(offset, sizeof (void*), old, &old);
}

void plt_set_offsets(plt_offset *offset, size_t nb_off, plt_fn *newval)
{
    for (size_t i = 0; i < nb_off; ++i) {
        if (!offset[i].oldval)
            offset[i].oldval = *offset[i].offset;
        plt_set_offset(offset[i].offset, newval);
    }
}

void plt_reset_offsets(plt_offset *offset, size_t nb_off)
{
    for (size_t i = 0; i < nb_off; ++i) {
        plt_set_offset(offset[i].offset, offset[i].oldval);
    }
}

plt_fn *plt_get_real_fn(plt_ctx ctx, const char *name)
{
    (void) ctx;
    DWORD errorMessageID;
    HANDLE snap;
    do {
        snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,
            GetCurrentProcessId());
        if (snap != INVALID_HANDLE_VALUE) {
            break;
        }
        errorMessageID = GetLastError();
    } while (errorMessageID == ERROR_BAD_LENGTH);

    mmk_assert(snap != INVALID_HANDLE_VALUE);

    MODULEENTRY32 mod = { .dwSize = sizeof(MODULEENTRY32) };
    for (BOOL more = Module32First(snap, &mod); more;
            more = Module32Next(snap, &mod))
    {
        FARPROC fn = GetProcAddress(mod.hModule, name);
        if (fn != NULL)
            return (plt_fn *) fn;
    }
    mmk_assert(GetLastError() == ERROR_NO_MORE_FILES);
    return NULL;
}
