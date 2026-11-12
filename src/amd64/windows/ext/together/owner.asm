.intel_syntax noprefix

.equ OWNER_ERR_ARG_NULL, 4
.equ OWNER_ERR_INVALID, 12
.equ OWNER_TEST_FRAME_BYTES, 0x100
.equ OWNER_TEST_CANARY, 0x13579bdf
.equ OWNER_TEST_RBX, 0x2468ace0
.equ OWNER_TEST_RSI, 0x31415926
.equ OWNER_TEST_RDI, 0x27182818
.equ OWNER_TEST_TAIL, 0x5a17c0de
.equ OWNER_DEFAULT_RANGE_BITS, 0x447a0000
.equ OWNER_DLL_LOAD_OK, 1

.text
.p2align 4
.globl better_together_owner_entry
.seh_proc better_together_owner_entry
better_together_owner_entry:
    .seh_endprologue
    mov eax, OWNER_DLL_LOAD_OK
    ret
.seh_endproc

.p2align 4
.globl better_together_owner_construct
.seh_proc better_together_owner_construct
better_together_owner_construct:
    push rbx
    .seh_pushreg rbx
    sub rsp, 0x20
    .seh_stackalloc 0x20
    .seh_endprologue
    mov eax, OWNER_ERR_ARG_NULL
    test r8, r8
    jz .Lowner_construct_done
    mov qword ptr [r8], 0
    test rcx, rcx
    jz .Lowner_construct_done
    test rdx, rdx
    jz .Lowner_construct_done
    mov rbx, r8
    call .Lowner_native
    mov qword ptr [rbx], rax
    xor eax, eax
.Lowner_construct_done:
    add rsp, 0x20
    pop rbx
    ret
.seh_endproc

.p2align 4
.seh_proc .Lowner_native
.Lowner_native:
    push rdi
    .seh_pushreg rdi
    sub rsp, 0x20
    .seh_stackalloc 0x20
    mov qword ptr [rsp + 0x30], rbx
    .seh_savereg rbx, 0x30
    mov qword ptr [rsp + 0x38], rsi
    .seh_savereg rsi, 0x38
    .seh_endprologue
    xor edi, edi
    mov rbx, rcx
    mov qword ptr [rcx], rdi
    mov qword ptr [rcx + 0x18], rdi
    mov qword ptr [rcx + 0x20], rdi
    mov qword ptr [rcx + 0x28], rdi
    mov dword ptr [rcx + 0x30], edi
    mov qword ptr [rcx + 0x38], rdi
    mov qword ptr [rcx + 0x40], rdi
    mov dword ptr [rcx + 0x48], OWNER_DEFAULT_RANGE_BITS
    mov qword ptr [rcx + 0x50], rdi
    mov qword ptr [rcx + 0x58], rdi
    mov qword ptr [rcx + 0x70], rdi
    mov qword ptr [rcx + 0x78], rdi
    mov qword ptr [rcx + 0x80], rdi
    mov qword ptr [rcx + 0x88], rdi
    mov byte ptr [rcx + 0xa8], dil
    jmp rdx
.seh_endproc

.p2align 4
.globl better_together_owner_test
.seh_proc better_together_owner_test
better_together_owner_test:
    push rbx
    .seh_pushreg rbx
    push rsi
    .seh_pushreg rsi
    push rdi
    .seh_pushreg rdi
    sub rsp, OWNER_TEST_FRAME_BYTES
    .seh_stackalloc OWNER_TEST_FRAME_BYTES
    .seh_endprologue
    mov ebx, OWNER_TEST_RBX
    mov esi, OWNER_TEST_RSI
    mov edi, OWNER_TEST_RDI
    mov qword ptr [rsp + 0x28], OWNER_TEST_CANARY
    mov qword ptr [rsp + 0xe0], OWNER_TEST_CANARY
    mov qword ptr [rsp + 0x30], OWNER_TEST_CANARY
    mov qword ptr [rsp + 0x40], 0
    mov dword ptr [rsp + 0x78], OWNER_TEST_CANARY
    mov byte ptr [rsp + 0xd8], 0xff
    lea rcx, [rsp + 0x30]
    lea rdx, [rip + .Lowner_test_tail]
    lea r8, [rsp + 0x20]
    call better_together_owner_construct
    test eax, eax
    jnz .Lowner_test_done
    mov eax, OWNER_ERR_INVALID
    cmp rbx, OWNER_TEST_RBX
    jne .Lowner_test_done
    cmp rsi, OWNER_TEST_RSI
    jne .Lowner_test_done
    cmp rdi, OWNER_TEST_RDI
    jne .Lowner_test_done
    cmp qword ptr [rsp + 0x28], OWNER_TEST_CANARY
    jne .Lowner_test_done
    cmp qword ptr [rsp + 0xe0], OWNER_TEST_CANARY
    jne .Lowner_test_done
    lea rcx, [rsp + 0x30]
    cmp qword ptr [rsp + 0x20], rcx
    jne .Lowner_test_done
    cmp qword ptr [rcx], 0
    jne .Lowner_test_done
    cmp qword ptr [rcx + 0x10], OWNER_TEST_TAIL
    jne .Lowner_test_done
    cmp dword ptr [rcx + 0x48], OWNER_DEFAULT_RANGE_BITS
    jne .Lowner_test_done
    cmp byte ptr [rcx + 0xa8], 0
    jne .Lowner_test_done
    xor ecx, ecx
    lea rdx, [rip + .Lowner_test_tail]
    lea r8, [rsp + 0x20]
    call better_together_owner_construct
    cmp eax, OWNER_ERR_ARG_NULL
    jne .Lowner_test_invalid
    cmp qword ptr [rsp + 0x20], 0
    jne .Lowner_test_invalid
    xor eax, eax
    jmp .Lowner_test_done
.Lowner_test_invalid:
    mov eax, OWNER_ERR_INVALID
.Lowner_test_done:
    add rsp, OWNER_TEST_FRAME_BYTES
    pop rdi
    pop rsi
    pop rbx
    ret
.seh_endproc

.p2align 4
.Lowner_test_tail:
    mov qword ptr [rbx + 0x10], OWNER_TEST_TAIL
    mov rax, rbx
    mov rbx, qword ptr [rsp + 0x30]
    mov rsi, qword ptr [rsp + 0x38]
    add rsp, 0x20
    pop rdi
    ret

.section .drectve
.ascii " -export:better_together_owner_construct"
.ascii " -export:better_together_owner_test"
