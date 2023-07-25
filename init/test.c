#define SYSCALL0(NUM)                                                          \
    ({                                                                         \
        int ret, errno;                                                        \
        asm volatile("syscall"                                                 \
                     : "=a"(ret), "=d"(errno)                                  \
                     : "a"(NUM)                                                \
                     : "rcx", "r11", "memory");                                \
    })
#define SYSCALL1(NUM, ARG0)                                                    \
    ({                                                                         \
        int ret, errno;                                                        \
        asm volatile("syscall"                                                 \
                     : "=a"(ret), "=d"(errno)                                  \
                     : "a"(NUM), "D"(ARG0)                                     \
                     : "rcx", "r11", "memory");                                \
    })

void _start() {
    asm volatile("mov rax, 0xC0FFEEBABE");
    SYSCALL1(0, "hello world lol\n");
    SYSCALL1(0, "meow");
    SYSCALL0(1);
    for (;;)
        ;
}
