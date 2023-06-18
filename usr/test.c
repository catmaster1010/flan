void _start(){
    asm volatile ("mov rax, 0xC0FFEBABE");
//    asm volatile ("syscall");
    for(;;);
}
