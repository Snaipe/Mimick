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
#include "mimick/matcher.h"
#include "vitals.h"
#include "threadlocal.h"
#include "mock.h"

static MMK_THREAD_LOCAL(struct mmk_result *) cur_result;

void mmk_when_init(struct mmk_result *res)
{
    tls_set(struct mmk_result *, cur_result, res);
}

struct mmk_result *mmk_when_get_result(void)
{
    return tls_get(struct mmk_result *, cur_result);
}

void mmk_when_impl(struct mmk_mock_ctx *mock, void *data)
{
    struct mmk_params *params = data;
    params->matcher_ctx = mmk_matcher_ctx();
    params->next = mock->params;
    mock->params = params;
}
