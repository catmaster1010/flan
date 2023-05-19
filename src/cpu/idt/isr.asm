extern isr
extern isr_generic
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

    %if %1 != 8 && %1 != 10 && %1 != 11 && %1 != 12 && %1 != 13 && %1 != 14 && %1 != 17 && %1 != 30
	 push qword 0
	%endif
    
    pushaq
    mov eax, ds
    push rax
    mov eax, es
    push rax

    cld
    mov rdi, %1
    mov rax, (%1 * 8)
    lea rbx, qword [isr]
    add rbx, rax
    mov rsi, rsp
    xor rbp, rbp
    call [rbx] 

    pop rax
    mov es, eax
    pop rax 
    mov ds, eax

    popaq
    add rsp, 8 
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
