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
start:
    call    next                                // Retrieve IP
next:
    pop     %eax

    push    %eax                                // Setup mock context
    mov     (start - next - 0x8)(%eax), %eax
    push    %eax                                // Call mmk_set_ctx
    mov     0x4(%eax), %eax
    call    *%eax
    pop     %eax

    mov     (%eax), %eax                        // Check if context was asked
    call    *%eax
    test    %eax, %eax
    jnz     ret_ctx

    pop     %eax
    mov     (start - next - 0x4)(%eax), %eax    // Retrieve offset at
                                                // the start of the map
    jmp     *%eax

ret_ctx:
    pop     %eax
    mov     (start - next - 0x8)(%eax), %eax
    mov     0x8(%eax), %eax                     // Call mmk_ctx
    call    *%eax
    ret

.globl mmk_trampoline_end
mmk_trampoline_end:
    nop
