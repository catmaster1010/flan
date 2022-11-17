global load_gdt
load_gdt:
        cli
        lgdt [rdi]
        mov ax, 0x30
        MOV   DS, AX
        MOV   ES, AX
        MOV   FS, AX
        MOV   GS, AX
        MOV   SS, AX
        pop rdi
        mov rax, 0x28
        push rax
        push rdi
        retfq


