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
.MODEL FLAT, C
.CODE

extern mmk_ctx:dword

mmk_trampoline label far
  call    next                                      ; Retrieve IP
next:
  pop     eax

  and     eax, 0fffff000h
  push    eax
  mov     eax, dword ptr [eax]                      ; Setup mock context
  push    eax                                       ; Call mmk_set_ctx
  call    dword ptr [eax + 4h]
  pop     eax

  call    dword ptr [eax]                           ; Check if context was asked
  test    eax, eax
  jnz     ret_ctx

  pop     eax                                       ; Retrieve offset at
  jmp     dword ptr [eax + 4h]                      ; the start of the map

ret_ctx:                                            ; Return context
  pop     eax
  mov     eax, dword ptr [eax]
  call    dword ptr [eax + 8h]                     ; Call mmk_ctx
  ret
mmk_trampoline_end label far

public mmk_trampoline
public mmk_trampoline_end

end
