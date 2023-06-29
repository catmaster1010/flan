void _start(){
    asm volatile ("mov rax, 0xC0FFEEBABE");
    asm volatile ("syscall");
    asm volatile ("syscall");
    for(;;);
}
