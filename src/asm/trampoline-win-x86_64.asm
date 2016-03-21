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
  push    rbx
  mov     rbx, mmk_ctx
  mov     [rbx], rax
  pop     rbx

  mov     rax, dword ptr [rax]                      ; Check if context was asked
  call    rax
  test    rax, rax
  jnz     ret_ctx

  pop     rax                                       ; Retrieve offset at
  jmp     qword ptr [rax + 8h]                      ; the start of the map

ret_ctx:                                            ; Return context
  mov     rax, mmk_ctx
  add     rsp, 8h
  ret
mmk_trampoline_end label far

public mmk_trampoline
public mmk_trampoline_end

end
