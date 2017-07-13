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

#include "mimick.h"
#include "mimick/assert.h"

#include "core.h"
#include "stub.h"
#include "threadlocal.h"
#include "trampoline.h"
#include "vitals.h"

static MMK_THREAD_LOCAL(int) ask_ctx;
static MMK_THREAD_LOCAL(struct mmk_stub *) mmk_ctx_;

void *mmk_stub_context(struct mmk_stub *stub)
{
    if (stub == MMK_STUB_INVALID)
        return NULL;
    return stub->ctx;
}

int mmk_stub_create_static(struct mmk_stub *stub,
        const char *target, mmk_fn fn, void *ctx)
{
    tls_set(int, ask_ctx, 0);
    tls_set(struct mmk_stub *, mmk_ctx_, NULL);

    char *name = mmk_malloc(mmk_strlen(target) + 1);
    if (!name)
        return -ENOMEM;

    mmk_strcpy(name, target);

    char *path = NULL;
    char *delim = mmk_strchr(name, '@');
    if (delim != NULL) {
        *delim = 0;
        path = delim + 1;
    }
    if (path && mmk_streq(path, "self"))
        path = NULL;

    plt_ctx pctx = mmk_plt_ctx();
    plt_lib lib = plt_get_lib(pctx, path);
    if (!lib)
        return -ENOENT;

    size_t nb_off = 0;
    plt_offset *off = plt_get_offsets(pctx, lib, name, &nb_off);
    if (!off || !nb_off)
        return -ENOENT;

    *stub = (struct mmk_stub) {
        .ctx_asked = mmk_ctx_asked,
        .ctx_set = mmk_set_ctx,
        .ctx_get = mmk_ctx,
        .ctx = ctx,
        .name = name,
        .path = path,
        .offsets = off,
        .nb_offsets = nb_off,
    };
    stub->trampoline = create_trampoline(stub, (plt_fn *) fn);
    plt_set_offsets(off, nb_off, stub->trampoline);

    return 0;
}

struct mmk_stub *mmk_stub_create(const char *target, mmk_fn fn, void *ctx)
{
    mmk_init();

    struct mmk_stub *stub = mmk_malloc(sizeof (struct mmk_stub));
    int rc = mmk_stub_create_static(stub, target, fn, ctx);
    if (rc < 0) {
        mmk_free (stub);
        errno = rc;
        stub = MMK_STUB_INVALID;
    }
    return stub;
}

void mmk_stub_destroy_static(struct mmk_stub *stub)
{
    plt_reset_offsets(stub->offsets, stub->nb_offsets);
    mmk_free(stub->offsets);
    destroy_trampoline(stub->trampoline);
    mmk_free(stub->name);
}

void mmk_stub_destroy(struct mmk_stub *stub)
{
    mmk_stub_destroy_static(stub);
    mmk_free(stub);
}

struct mmk_stub *mmk_ask_ctx(mmk_fn fn)
{
    tls_set(int, ask_ctx, 1);
    return ((struct mmk_stub *(*)(void)) fn)();
}

int mmk_ctx_asked(void)
{
    int asked = tls_get(int, ask_ctx);
    tls_set(int, ask_ctx, 0);
    return asked;
}

struct mmk_stub *mmk_ctx(void)
{
    return tls_get(struct mmk_stub *, mmk_ctx_);
}

void mmk_set_ctx(struct mmk_stub *stub)
{
    tls_set(struct mmk_stub *, mmk_ctx_, stub);
}
