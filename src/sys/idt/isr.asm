extern isr
extern isr_generic
 
%macro stub 1
isr_stub_%1:

    %if %1 != 8 && %1 != 10 && %1 != 11 && %1 != 12 && %1 != 13 && %1 != 14 && %1 != 17 && %1 != 30
	 push qword 0
	%endif
    
    cmp qword [rsp + 16], 0x3b
    jne .swap
    swapgs

    .swap:
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

    mov eax, ds
    push rax
    mov eax, es
    push rax

    cld

    mov rax, 0x30
    mov ds, eax
    mov es, eax
    mov ss, eax
    
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
    add rsp, 8 

    cmp qword [rsp + 8], 0x3b
    jne .swap1
    swapgs

    .swap1:
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
