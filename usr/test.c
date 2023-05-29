void _start(){
    asm volatile ("mov rax, 69");
    asm volatile ("syscall");
}
