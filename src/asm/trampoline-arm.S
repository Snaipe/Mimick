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

.globl mmk_trampoline
mmk_trampoline:
    adr     ip, .

    push    {r0, r1, r2, r3}
    push    {ip, lr}

    ldr     r0, [ip, #-8]                       // Call mmk_set_ctx
    ldr     ip, [r0, #4]
    blx     ip

    pop     {ip, lr}
    push    {ip, lr}

    ldr     ip, [ip, #-8]                       // Check if context was asked
    ldr     ip, [ip]
    blx     ip
    cmp     r0, #0
    bne     ret_ctx

    pop     {ip, lr}
    pop     {r0, r1, r2, r3}

    ldr     pc, [ip, #-4]                       // Trampoline jump

ret_ctx:

    pop     {ip, lr}
    pop     {r0, r1, r2, r3}
    push    {ip, lr}

    ldr     ip, [ip, #-8]                       // Call mmk_ctx
    ldr     ip, [ip, #8]
    blx     ip

    pop     {ip, pc}

.globl mmk_trampoline_end
mmk_trampoline_end:
    nop
