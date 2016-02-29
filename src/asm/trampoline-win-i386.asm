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
.386
.MODEL FLAT
.CODE

extern _mmk_ctx:dword

_mmk_trampoline proc
option prologue:none, epilogue:none
start:
  call    next                                      ; Retrieve IP
next:
  pop     eax

  add     eax, start
  sub     eax, next
  push    eax                                       ; Setup mock context
  mov     eax, dword ptr [eax - 8h]
  mov     _mmk_ctx, eax

  pop     eax
  mov     eax, dword ptr [eax - 4h]                 ; Retrieve offset at
                                                    ; the start of the map
  jmp     dword ptr [eax]
_mmk_trampoline endp

_mmk_trampoline_end proc
option prologue:none, epilogue:none
  nop
_mmk_trampoline_end endp

end
