;
; The MIT License (MIT)
;
; Copyright © 2016 Franklin "Snaipe" Mathieu <http://snai.pe/>
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.
;
.CODE

PUSHDQ MACRO Reg;
    sub rsp, 10h
    movdqu xmmword ptr [rsp], Reg
ENDM

POPDQ MACRO Reg;
    movdqu Reg, xmmword ptr [rsp]
    add rsp, 10h
ENDM

; mmk_ctx defined as proc instead of qword to avoid
; MASM's relative addressing bullshit
extern mmk_ctx:proc

mmk_trampoline label far
  call    next                                      ; Retrieve IP
next:
  pop     rax

  and     rax, 0fffffffffffff000h
  push    rax
  mov     rax, qword ptr [rax]                      ; Setup mock context

  push    rcx                                       ; Save caller ctx
  push    rdx
  push    r8
  push    r9
  PUSHDQ  xmm0
  PUSHDQ  xmm1
  PUSHDQ  xmm2
  PUSHDQ  xmm3

  mov     rcx, rax                                  ; Call mmk_set_ctx
  push    rax
  call    qword ptr [rax + 8h]
  pop     rax

  call    qword ptr [rax]                           ; Check if context was asked

  POPDQ   xmm3                                      ; Restore caller ctx
  POPDQ   xmm2
  POPDQ   xmm1
  POPDQ   xmm0
  pop     r9
  pop     r8
  pop     rdx
  pop     rcx

  test    rax, rax
  jnz     ret_ctx

  pop     rax                                       ; Retrieve offset at
  jmp     qword ptr [rax + 8h]                      ; the start of the map

ret_ctx:                                            ; Return context
  pop     rax
  mov     rax, qword ptr [rax]
  ret
mmk_trampoline_end label far

public mmk_trampoline
public mmk_trampoline_end

end
