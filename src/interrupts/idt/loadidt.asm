%macro isr_err_stub 1
isr_stub_%1:
    pushal
    call exception_handler ;Located in IDT.c
    popal
    iretq ;Preform long jump.
%endmacro

%macro isr_no_err_stub 1
isr_stub_%1: 
    pushal
    call exception_handler ;Located in IDT.c
    popal
    iretq ;Preform long jump. 
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
    
extern exception_handler
global load_idt
global isr_stub_table

load_idt:
    MOV   [idtr], DI
    MOV  [idtr+2], RSI
    LIDT  [idtr] ;Load IDT into cpu.
    sti ; Enable interrupts again.
    ret

;IRETQ (Return from interrupts) POPS RIP AND CS (RIP:CS)


extern exception_handler
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

;Stub table which prevents execssive code use


section	.data
idtr DW 0
DQ 0 

isr_stub_table:
%assign i 0
%rep 32
    DQ isr_stub_%+i 
%assign i i+1
%endrep