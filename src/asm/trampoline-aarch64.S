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
    adr     x16, .

    stp     x7, x6, [sp, #-16]!
    stp     x5, x4, [sp, #-16]!
    stp     x3, x2, [sp, #-16]!
    stp     x1, x0, [sp, #-16]!

    stp     x16, x30, [sp, #-16]!

    ldr     x0, [x16, #-16]                     // Call mmk_set_ctx
    ldr     x16, [x0, #8]
    blr     x16

    ldp     x16, x30, [sp]

    ldr     x16, [x16, #-16]                    // Check if context was asked
    ldr     x16, [x16]
    blr     x16
    cmp     x0, #0
    b.ne    ret_ctx

    ldp     x16, x30, [sp], #16
    ldp     x1, x0, [sp], #16
    ldp     x3, x2, [sp], #16
    ldp     x5, x4, [sp], #16
    ldp     x7, x6, [sp], #16

    ldr     x16, [x16, #-8]                     // Trampoline jump
    br      x16

ret_ctx:

    ldp     x16, x30, [sp], #16
    ldp     x1, x0, [sp], #16
    ldp     x3, x2, [sp], #16
    ldp     x5, x4, [sp], #16
    ldp     x7, x6, [sp], #16
    stp     x16, x30, [sp, #-16]!

    ldr     x16, [x16, #-16]                    // Call mmk_ctx
    ldr     x16, [x16, #16]
    blr     x16

    ldp     x16, x30, [sp], #16
    ret

.globl mmk_trampoline_end
mmk_trampoline_end:
    nop
