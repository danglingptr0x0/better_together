.intel_syntax noprefix
.text

.equ BIND_ERR_ARG_NULL, 4
.equ BIND_OWNER_BACK_OFF, 8

.globl better_together_setup_bind_layout_get
better_together_setup_bind_layout_get:
    test rcx, rcx
    jz .Lbind_layout_null
    lea rax, [rip + .Lbind]
    mov [rcx], rax
    lea rdx, [rip + .Lbind_end]
    sub rdx, rax
    mov [rcx + 8], rdx
    lea rdx, [rip + .Lbind_code_end]
    sub rdx, rax
    mov [rcx + 16], rdx
    lea rdx, [rip + .Lbind_owner]
    sub rdx, rax
    mov [rcx + 24], rdx
    lea rdx, [rip + .Lbind_orig]
    sub rdx, rax
    mov [rcx + 32], rdx
    lea rdx, [rip + .Lbind_unwind]
    sub rdx, rax
    mov [rcx + 40], rdx
    xor eax, eax
    ret
.Lbind_layout_null:
    mov eax, BIND_ERR_ARG_NULL
    ret

.p2align 4
.Lbind:
    mov rax, [rip + .Lbind_owner]
    mov rax, [rax]
    test rax, rax
    jz .Lbind_forward
    cmp rcx, [rax + BIND_OWNER_BACK_OFF]
    jne .Lbind_forward
    xor r8d, r8d
    xor r9d, r9d
.Lbind_forward:
    mov rax, [rip + .Lbind_orig]
    jmp qword ptr [rax]
.Lbind_code_end:
.p2align 3
.Lbind_owner:
    .quad 0
.Lbind_orig:
    .quad 0
.p2align 2
.Lbind_unwind:
    .byte 1, 0, 0, 0
.Lbind_end:

.equ IK_SEGMENTS_OFF, 0x1e0
.equ IK_CAP_OFF, 0x1e8
.equ IK_CURRENT_OFF, 0x204
.equ IK_GRIP_OFF, 0x438
.equ IK_FIRST_EN_OFF, 0x48
.equ IK_SECOND_EN_OFF, 0x50

.globl better_together_setup_ik_layout_get
better_together_setup_ik_layout_get:
    test rcx, rcx
    jz .Lik_layout_null
    lea rax, [rip + .Lik]
    mov [rcx], rax
    lea rdx, [rip + .Lik_end]
    sub rdx, rax
    mov [rcx + 8], rdx
    lea rdx, [rip + .Lik_code_end]
    sub rdx, rax
    mov [rcx + 16], rdx
    lea rdx, [rip + .Lik_owner]
    sub rdx, rax
    mov [rcx + 24], rdx
    lea rdx, [rip + .Lik_orig]
    sub rdx, rax
    mov [rcx + 32], rdx
    lea rdx, [rip + .Lik_unwind]
    sub rdx, rax
    mov [rcx + 40], rdx
    xor eax, eax
    ret
.Lik_layout_null:
    mov eax, BIND_ERR_ARG_NULL
    ret

.p2align 4
.Lik:
    mov rax, [rip + .Lik_owner]
    mov rax, [rax]
    test rax, rax
    jz .Lik_forward
    cmp rcx, [rax + BIND_OWNER_BACK_OFF]
    jne .Lik_forward
    cmp qword ptr [rcx + IK_SEGMENTS_OFF], 0
    je .Lik_disable
    cmp dword ptr [rcx + IK_CAP_OFF], 0
    jle .Lik_disable
    mov eax, [rcx + IK_CURRENT_OFF]
    cmp eax, [rcx + IK_CAP_OFF]
    jae .Lik_disable
    mov eax, [rcx + IK_GRIP_OFF]
    cmp eax, [rcx + IK_CAP_OFF]
    jae .Lik_disable
    jmp .Lik_forward
.Lik_disable:
    mov byte ptr [rsp + IK_FIRST_EN_OFF], 0
    mov byte ptr [rsp + IK_SECOND_EN_OFF], 0
.Lik_forward:
    mov rax, [rip + .Lik_orig]
    jmp qword ptr [rax]
.Lik_code_end:
.p2align 3
.Lik_owner:
    .quad 0
.Lik_orig:
    .quad 0
.p2align 2
.Lik_unwind:
    .byte 1, 0, 0, 0
.Lik_end:
