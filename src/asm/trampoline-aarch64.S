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
#include "mangling.h"

.align 8
.globl MANGLE(mmk_trampoline)
MANGLE(mmk_trampoline):
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

.align 8
.globl MANGLE(mmk_trampoline_end)
MANGLE(mmk_trampoline_end):
    nop

.align 8
.globl MANGLE(mmk_clear_cache)
MANGLE(mmk_clear_cache):
cache_start:
	sub	sp, sp, #80
	str	x0, [sp, 8]
	str	x1, [sp]
	ldr	x0, [sp, 8]
	str	x0, [sp, 56]
	ldr	x1, [sp, 56]
	ldr	x0, [sp]
	add	x0, x1, x0
	str	x0, [sp, 48]
	mrs x0, ctr_el0                         // Fetching cache type for Data/Instruction Cache
	str	x0, [sp, 40]
	ldr	x0, [sp, 40]
	lsr	x1, x0, 16
	mov	w0, w1
	and	w0, w0, 15
	mov	w1, 4
	lsl	w0, w1, w0
	sxtw	x0, w0
	str	x0, [sp, 32]
	ldr	x0, [sp, 56]
	b	cache_cvau
cvau_loop:
	ldr	x0, [sp, 72]
	dc cvau, x0				// Invalidate Data cache line with 64 bits register
	ldr	x1, [sp, 72]
	ldr	x0, [sp, 32]
	add	x0, x1, x0
	str	x0, [sp, 72]
cache_cvau:
	ldr	x1, [sp, 72]
	ldr	x0, [sp, 48]
	cmp	x1, x0
	bcc	cvau_loop
	dsb ish
	ldr	x0, [sp, 40]
	and	w0, w0, 15
	mov	w1, 4
	lsl	w0, w1, w0
	sxtw	x0, w0
	str	x0, [sp, 24]
	ldr	x0, [sp, 56]
	str	x0, [sp, 64]
	b	cache_isb
cache_ivau:
	ldr	x0, [sp, 64]
	ic ivau, x0				// Invalidate Instruction cache line withh 64 bits register
	ldr	x1, [sp, 64]
	ldr	x0, [sp, 24]
	add	x0, x1, x0
	str	x0, [sp, 64]
cache_isb:
	ldr	x1, [sp, 64]
	ldr	x0, [sp, 48]
	cmp	x1, x0
	bcc	cache_ivau
	isb sy
	mov	w0, 0
	add	sp, sp, 80
	ret
