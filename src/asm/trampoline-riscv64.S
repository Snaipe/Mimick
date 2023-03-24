/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Ziyao <ziyao@disroot.org>
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

	.global		MANGLE(mmk_trampoline), MANGLE(mmk_trampoline_end)
MANGLE(mmk_trampoline):

/*
 *	struct mmk_stub*		8bytes
 *	plt_fn*				8bytes
 *	trampoline code			...
 *	=>
 *		start: jal t0, next	4bytes
 */
start:
	jal	t0,	next			// 4 bytes for an instruction
next:
	add	sp,	sp,	-8 * 12		// a0 - a7 + t0 + ra +
						// *struct mmk_stub aligned to
						// 16bytes
	sd	t0,	0(sp)			// 0(sp) label next
	ld	t0,	-20(t0)			// t0 -> struct mmk_stub*
	sd	t0,	8(sp)			// 8(sp) struct mmk_stub*
	sd	ra,	16(sp)

	/*	Save caller arguments	*/
	sd	a0,	24(sp)
	sd	a1,	32(sp)
	sd	a2,	40(sp)
	sd	a3,	48(sp)
	sd	a4,	56(sp)
	sd	a5,	64(sp)
	sd	a6,	72(sp)
	sd	a7,	80(sp)
	//	8 bytes padding
	/*	TODO:	float registers		*/

	mv	a0,	t0
	ld	ra,	8(t0)			// ctx_set
	jalr	ra

	ld	t0,	8(sp)

	ld	ra,	0(t0)			// ctx_asked
	jalr	ra
	mv	t0,	a0

	/*	Restore caller arguments	*/
	ld	a0,	24(sp)
	ld	a1,	32(sp)
	ld	a2,	40(sp)
	ld	a3,	48(sp)
	ld	a4,	56(sp)
	ld	a5,	64(sp)
	ld	a6,	72(sp)
	ld	a7,	80(sp)

	bnez	t0,	return_context
	/*	stub trampoline jump	*/
	ld	t0,	0(sp)
	ld	t0,	-12(t0)
	ld	ra,	16(sp)
	add	sp,	sp,	8 * 12
	jr	t0

return_context:
	ld	t0,	8(sp)
	ld	ra,	16(t0)			// struct mmk_stub* -> ctx_get()
	jalr	ra
	ld	ra,	16(sp)
	add	sp,	sp,	8 * 12
	ret

MANGLE(mmk_trampoline_end):
	nop
