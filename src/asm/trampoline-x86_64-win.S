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

# define PUSHDQ(Reg) \
    sub $0x10, %rsp; \
    movdqu Reg, (%rsp)

# define POPDQ(Reg) \
    movdqu (%rsp), Reg; \
    add $0x10, %rsp

.globl mmk_trampoline
mmk_trampoline:
start:
    call    next                                // Retrieve IP
next:
    pop     %rax

    push    %rax                                // Setup mock contextscratch registers
    mov     (start - next - 0x10)(%rax), %rax

    push    %rdx                                // Save caller ctx
    push    %rcx
    push    %r8
    push    %r9
    PUSHDQ  (%xmm0)
    PUSHDQ  (%xmm1)
    PUSHDQ  (%xmm2)
    PUSHDQ  (%xmm3)

    mov     %rax, %rcx                          // Call mmk_set_ctx
    push    %rax
    mov     0x8(%rax), %rax
    call    *%rax
    pop     %rax

    mov     (%rax), %rax                        // Check if context was asked
    call    *%rax

    POPDQ   (%xmm3)                             // Restore caller ctx
    POPDQ   (%xmm2)
    POPDQ   (%xmm1)
    POPDQ   (%xmm0)
    pop     %r9
    pop     %r8
    pop     %rcx
    pop     %rdx

    test    %rax, %rax
    jnz     ret_ctx

    pop     %rax
    mov     (start - next - 0x8)(%rax), %rax    // Retrieve offset at
                                                // the start of the map
    jmp     *%rax

ret_ctx:                                        // Return context
    pop     %rax
    mov     (start - next - 0x10)(%rax), %rax
    mov     0x10(%rax), %rax                    // Call mmk_ctx
    call    *%rax
    ret

.globl mmk_trampoline_end
mmk_trampoline_end:
    nop
