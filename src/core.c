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

#include "mimick.h"

#include "assert.h"
#include "mock.h"
#include "plt.h"
#include "stub.h"
#include "vitals.h"

static struct {
    int initialized;
    plt_ctx plt;
} self;

void mmk_init (void)
{
    if (self.initialized)
        return;

    self.plt = plt_init_ctx();
    mmk_assert (self.plt != (void*) -1);

    mmk_init_vital_functions (self.plt);

    self.initialized = 1;
}

plt_ctx mmk_plt_ctx (void)
{
    mmk_assert(self.initialized);
    return self.plt;
}

#undef mmk_reset
void mmk_reset (mmk_fn fn)
{
    struct mmk_stub *stub = mmk_ask_ctx (fn);
    struct mmk_mock_ctx *mock = mmk_stub_context (stub);

    mmk_mock_destroy_internal (mock);
}
