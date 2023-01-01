
extern isr

%macro stub 1
global isr_stubs%1
isr_stubs_%1:
%if %1 != 8 && %1 != 10 && %1 != 11 && %1 != 12 && %1 != 13 && %1 != 14 && %1 != 17 && %1 != 30
    push 0
%endif
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    mov eax, es
    push rax
    mov eax, ds
    push rax

    cld

    mov eax, 0x30
    mov ds, eax
    mov es, eax
    mov ss, eax

    mov rdi, %1
    mov rax, (%1 * 8)
    lea rbx, qword [isr]
    add rbx, rax
    mov rsi, rsp
    xor rbp, rbp
    call [rbx] ; *(%rbx)

    pop rax
    mov ds, eax
    pop rax
    mov es, eax
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    add rsp, 8
    iretq
%endmacro

%macro stub_address 1
dq isr_stubs_%1
%endmacro

section .data

global isr_stubs
align 8
isr_stubs:
%assign i 0
%rep 256
    stub_address i
    %assign i i+1
%endrep

section .text

%assign i 0
%rep 256
    stub i
    %assign i i+1
%endrep