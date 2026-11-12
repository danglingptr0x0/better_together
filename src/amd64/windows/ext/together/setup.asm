.intel_syntax noprefix

.equ SETUP_ERR_ARG_NULL, 4
.equ SETUP_LAYOUT_BYTES, 48

.text
.p2align 4
.globl better_together_setup_layout_get
.seh_proc better_together_setup_layout_get
better_together_setup_layout_get:
    .seh_endprologue
    mov eax, SETUP_ERR_ARG_NULL
    test rcx, rcx
    jz .Lsetup_layout_done
    lea rax, [rip + .Lsetup_entry]
    mov qword ptr [rcx], rax
    lea rax, [rip + .Lsetup_entry_end]
    lea rdx, [rip + .Lsetup_entry]
    sub rax, rdx
    mov qword ptr [rcx + 8], rax
    lea rax, [rip + .Lsetup_entry_code_end]
    lea rdx, [rip + .Lsetup_entry]
    sub rax, rdx
    mov qword ptr [rcx + 16], rax
    lea rax, [rip + .Lsetup_entry_ctx]
    lea rdx, [rip + .Lsetup_entry]
    sub rax, rdx
    mov qword ptr [rcx + 24], rax
    lea rax, [rip + .Lsetup_entry_cb]
    lea rdx, [rip + .Lsetup_entry]
    sub rax, rdx
    mov qword ptr [rcx + 32], rax
    lea rax, [rip + .Lsetup_entry_unwind]
    lea rdx, [rip + .Lsetup_entry]
    sub rax, rdx
    mov qword ptr [rcx + 40], rax
    add rcx, SETUP_LAYOUT_BYTES
    lea rax, [rip + .Lsetup_rope]
    mov qword ptr [rcx], rax
    lea rax, [rip + .Lsetup_rope_end]
    lea rdx, [rip + .Lsetup_rope]
    sub rax, rdx
    mov qword ptr [rcx + 8], rax
    lea rax, [rip + .Lsetup_rope_code_end]
    lea rdx, [rip + .Lsetup_rope]
    sub rax, rdx
    mov qword ptr [rcx + 16], rax
    lea rax, [rip + .Lsetup_rope_ctx]
    lea rdx, [rip + .Lsetup_rope]
    sub rax, rdx
    mov qword ptr [rcx + 24], rax
    lea rax, [rip + .Lsetup_rope_cb]
    lea rdx, [rip + .Lsetup_rope]
    sub rax, rdx
    mov qword ptr [rcx + 32], rax
    lea rax, [rip + .Lsetup_rope_unwind]
    lea rdx, [rip + .Lsetup_rope]
    sub rax, rdx
    mov qword ptr [rcx + 40], rax
    xor eax, eax
.Lsetup_layout_done:
    ret
.seh_endproc

.p2align 4
.Lsetup_entry:
    mov r9, qword ptr [rip + .Lsetup_entry_ctx]
    jmp qword ptr [rip + .Lsetup_entry_cb]
.Lsetup_entry_code_end:
.p2align 3
.Lsetup_entry_ctx:
    .quad 0
.Lsetup_entry_cb:
    .quad 0
.p2align 2
.Lsetup_entry_unwind:
    .byte 1, 0, 0, 0
.Lsetup_entry_end:

.p2align 4
.Lsetup_rope:
    sub rsp, 0x28
    mov rax, qword ptr [rip + .Lsetup_rope_ctx]
    mov qword ptr [rsp + 0x20], rax
    call qword ptr [rip + .Lsetup_rope_cb]
    add rsp, 0x28
    ret
.Lsetup_rope_code_end:
.p2align 3
.Lsetup_rope_ctx:
    .quad 0
.Lsetup_rope_cb:
    .quad 0
.p2align 2
.Lsetup_rope_unwind:
    .byte 1, 4, 1, 0, 4, 0x42, 0, 0
.Lsetup_rope_end:
