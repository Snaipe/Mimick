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
#include "vitals.h"

enum plt_selector plt_get_selector(const char *target, const char **val)
{
    enum plt_selector sel = PLT_SEL_NONE;
    if (mmk_strneq(target, "lib:", 4))
        sel = PLT_SEL_LIB;
    else if (mmk_strneq(target, "file:", 5))
        sel = PLT_SEL_FILE;
    else if (mmk_strneq(target, "sym:", 4))
        sel = PLT_SEL_SYM;
    else if (mmk_streq(target, "self"))
        target = "";
    else {

        char *end_sel = mmk_strchr(target, ':');
        if (end_sel) {
            size_t len = (size_t) (end_sel - target);
            mmk_panic("mimick: unknown '%.*s' selector.\n", (int) len, target);
        } else {
            mmk_panic("mimick: unknown target kind '%s'.\n", target);
        }
    }

    *val = sel == PLT_SEL_NONE ? target : mmk_strchr(target, ':') + 1;
    return sel;
}
