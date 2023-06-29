extern syscall_handler
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

    mov eax, ds
    push rax
    mov eax, es
    push rax

    cld

    mov rax, 0x30
    mov ds, eax
    mov es, eax

    call syscall_handler
    
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
    add rsp, 0x30 ; err, rip, cs, rflags, rsp, ss
    pop rsp ; get old stack back
    swapgs
    o64 sysret
