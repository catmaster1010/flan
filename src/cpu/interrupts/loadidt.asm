extern exception_handler
%macro isr_err_stub 1
isr_stub_%1:
    ;pushal
    call exception_handler ;Located in IDT.c ; It is written with no return
   ;popal
    iretq ;Preform long jump.
%endmacro

%macro isr_no_err_stub 1
isr_stub_%1: 
    ;pushal
    call exception_handler ;Located in IDT.c
    ;popal
    iretq
%endmacro
;save state
%macro pushal 0
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

;restore state
%macro popal 0 
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

section	.text


isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

section	.data
global isr_stub_table
global load_idt

isr_stub_table:
%assign i 0
%rep 32
    dq isr_stub_%+i 
%assign i i+1
%endrep


;IRETQ (Return from interrupts) POPS RIP  CS (RIP:CS) -- quad since 64 bit.

