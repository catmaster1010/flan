extern isr

%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

 
%macro stub 1
isr_stub_%1:
    pushaq
    
    cld
    mov rdi, %1
    mov rax, (%1 * 8)
    lea rbx, qword [isr]
    add rbx, rax
    mov rsi, rsp
    xor rbp, rbp
    call [rbx] 

    popaq
    iretq
%endmacro

%macro stub_address 1
dq isr_stub_%1
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
