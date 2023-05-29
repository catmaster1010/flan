
extern syscall_handler
global syscall_entry_asm
syscall_entry_asm:
    call syscall_handler
    o64 sysret
