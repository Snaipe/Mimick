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
#ifndef PLT_H_
# define PLT_H_

# include <stddef.h>

# include "config.h"

# if defined MMK_EXE_FMT_ELF
#  include "plt-elf.h"
# elif defined MMK_EXE_FMT_PE
#  include "plt-pe.h"
# elif defined MMK_EXE_FMT_MACH_O
#  include "plt-mach-o.h"
# endif

typedef struct plt_offset {
    plt_fn **offset;
    plt_fn *oldval;
} plt_offset;

enum plt_selector {
    PLT_SEL_NONE, PLT_SEL_LIB, PLT_SEL_FILE, PLT_SEL_SYM
};

plt_ctx plt_init_ctx(void);
plt_lib plt_get_lib(plt_ctx ctx, const char *name);
plt_offset *plt_get_offsets(plt_ctx ctx, plt_lib lib, const char *name, size_t *n);
void plt_set_offsets(plt_offset *offset, size_t nb_off, plt_fn *newval);
void plt_reset_offsets(plt_offset *offset, size_t nb_off);
plt_fn *plt_get_real_fn(plt_ctx ctx, const char *name);
enum plt_selector plt_get_selector(const char *target, const char **val);

#endif /* !PLT_H_ */
