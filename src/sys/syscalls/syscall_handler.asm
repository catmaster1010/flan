section .rodata

extern syscall_log
extern syscall_mmap
extern syscall_exit_thread
syscall_table:
    dq syscall_log  ;1
    dq syscall_mmap ;2
    dq syscall_exit_thread ;3

section .text

global syscall_entry_asm
syscall_entry_asm:

    swapgs

    xchg rsp, [gs:0]
    push qword [gs:0] ; push old stack
    mov [gs:0], rsp 
    add qword [gs:0], 8 
    
    push qword 0x43 ; ss
    push qword [rsp+8] ; rsp
    push r11 ; rflags	
    push qword 0x3b ; cs
    push rcx ; rip
    push qword 0 ; err
    
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

    mov r11, ds
    push r11
    mov r11, es
    push r11

    mov rcx, r10

    cld

    mov r11, 0x30
    mov ds, r11
    mov es, r11

    call [syscall_table + rax * 8]
    
    pop r11
    mov es, r11
    pop r11
    mov ds, r11

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
    add rsp, 0x8 ; rdx
    pop rcx
    pop rbx
    add rsp, 0x38 ; rax, err, rip, cs, rflags, rsp, ss
    pop rsp ; get old stack back
    cli
    swapgs
    o64 sysret
