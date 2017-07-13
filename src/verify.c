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
#include "mimick/verify.h"

#include "mock.h"
#include "threadlocal.h"
#include "vitals.h"

static MMK_THREAD_LOCAL(size_t) times;

void mmk_verify_set_times(size_t t)
{
    tls_set(size_t, times, t);
}

int mmk_verify_times(struct mmk_verify_params *params)
{
    size_t t = tls_get(size_t, times);
    if (params->never)
        return t == 0;
    if (params->at_least || params->at_most) {
        size_t most = params->at_most ? params->at_most : t;
        return t >= params->at_least && t <= most;
    }
    if (params->matching)
        return params->matching(t);
    if (params->times > 0)
        return params->times == t;
    return 1;
}

static int find_and_inc_call_matching(struct mmk_mock_ctx *mock,
        void *params, size_t size)
{
    // skip .times field
    params = (void*) ((char *) params + sizeof (size_t));
    size -= sizeof (size_t);

    for (void *p = mmk_mock_params_begin(mock); p;
            p = mmk_mock_params_next(mock, p))
    {
        // compare parameters w/o .times
        int res = mmk_memcmp((char *) p + sizeof (size_t), params, size);
        if (!res) {
            size_t *times = p;
            ++*times;
            return 1;
        }
    }
    return 0;
}

void mmk_verify_register_call(void *params, size_t size)
{
    struct mmk_mock_ctx *mock = mmk_stub_context(mmk_ctx());
    if (!mock->call_data) {
        mock->call_data = mmk_malloc(4096);
        mmk_assert(mock->call_data);
        mock->call_data_size = 4096;
    }

    if (find_and_inc_call_matching(mock, params, size))
        return;

    if (mock->call_data_top + size + sizeof (size_t) >= mock->call_data_size) {
        while (mock->call_data_top + size + sizeof (size_t) >= mock->call_data_size) {
            mock->call_data_size += 4096;
        }
        mock->call_data = mmk_realloc(mock->call_data, mock->call_data_size);
        mmk_assert(mock->call_data);
    }

    mmk_memcpy(mock->call_data + mock->call_data_top, &size, sizeof (size_t));
    size_t *times = mmk_memcpy(mock->call_data + mock->call_data_top
            + sizeof (size_t), params, size);
    *times = 1;

    mock->call_data_top += size + sizeof (size_t);
}
