global flush_gdt
flush_gdt:
        cli
        lgdt [rdi]
        mov ax, 0x30
        mov ss, ax
        mov ds, ax
        mov es, ax
        pop rdi
        mov rax, 0x28
        push rax
        push rdi
        retfq


