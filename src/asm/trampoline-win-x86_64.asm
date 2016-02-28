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

extern _mmk_ctx:qword

mmk_trampoline proc
option prologue:none, epilogue:none
start:
  call    next                                      ; Retrieve IP
next:
  pop     rax

  push    rax                                       ; Setup mock context
  mov     rax, qword ptr [rax + start - next - 10h]
  mov     mmk_ctx, rax

  pop     rax
  mov     rax, qword ptr [rax + start - next - 8h]  ; Retrieve offset at
                                                    ; the start of the map
  jmp     qword ptr [rax]
mmk_trampoline endp

mmk_trampoline_end proc
option prologue:none, epilogue:none
  nop
mmk_trampoline_end endp

end
