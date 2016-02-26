.globl mmk_trampoline
mmk_trampoline:
start:
    call    next                                // Retrieve IP
next:
    pop     %rax

    push    %rax                                // Setup mock context
    mov     (start - next - 0x10)(%rax), %rax
    mov     %rax, mmk_ctx

    pop     %rax
    mov     (start - next - 0x8)(%rax), %rax    // Retrieve offset at
                                                // the start of the map
    jmp     *%rax

.globl mmk_trampoline_end
mmk_trampoline_end:
    nop
