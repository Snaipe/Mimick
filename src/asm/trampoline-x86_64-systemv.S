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

# define PUSHDQ(Reg) \
    sub $0x10, %rsp; \
    movdqu Reg, (%rsp)

# define POPDQ(Reg) \
    movdqu (%rsp), Reg; \
    add $0x10, %rsp

.globl MANGLE(mmk_trampoline)
MANGLE(mmk_trampoline):
start:
    call    next                                // Retrieve IP
next:
    pop     %r11

    push    %r11                                // Setup mock context
    mov     (start - next - 0x10)(%r11), %r11

    push    %rdi                                // Save caller ctx
    push    %rsi
    push    %rdx
    push    %rcx
    push    %r8
    push    %r9
    PUSHDQ  (%xmm0)
    PUSHDQ  (%xmm1)
    PUSHDQ  (%xmm2)
    PUSHDQ  (%xmm3)
    PUSHDQ  (%xmm4)
    PUSHDQ  (%xmm5)
    PUSHDQ  (%xmm6)
    PUSHDQ  (%xmm7)

    push    %rax

    mov     %r11, %rdi                          // Call mmk_set_ctx
    push    %r11
    mov     0x8(%r11), %r11
    call    *%r11
    pop     %r11

    mov     (%r11), %r11                        // Check if context was asked
    call    *%r11

    mov     %rax, %r11
    pop     %rax

    POPDQ   (%xmm7)                             // Restore caller ctx
    POPDQ   (%xmm6)
    POPDQ   (%xmm5)
    POPDQ   (%xmm4)
    POPDQ   (%xmm3)
    POPDQ   (%xmm2)
    POPDQ   (%xmm1)
    POPDQ   (%xmm0)
    pop     %r9
    pop     %r8
    pop     %rcx
    pop     %rdx
    pop     %rsi
    pop     %rdi

    test    %r11, %r11
    jnz     ret_ctx

    pop     %r11
    mov     (start - next - 0x8)(%r11), %r11    // Retrieve offset at
                                                // the start of the map
    jmp     *%r11

ret_ctx:                                        // Return context
    pop     %r11
    mov     (start - next - 0x10)(%r11), %r11
    mov     0x10(%r11), %r11                    // Call mmk_ctx
    call    *%r11
    ret

.globl MANGLE(mmk_trampoline_end)
MANGLE(mmk_trampoline_end):
    nop
