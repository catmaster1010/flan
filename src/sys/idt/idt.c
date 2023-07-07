#include "idt.h"
#include "sys/cpu.h"
#include "sys/idt/idt.h"
#include "sys/smp.h"
#include "lib/lock.h"
#include "lib/stdio.h"

spinlock_t idt_lock = LOCK_INIT;
static idt_gate_t idt[256];
void *isr[256];
extern void *isr_stubs[];
static idtptr_t idtr;

const char *exception_names[] = {"Division By Zero",
                                 "Debug",
                                 "NMI",
                                 "Breakpoint",
                                 "Overflow",
                                 "Bound Range Exceeded",
                                 "Invalid Opcode",
                                 "Device Not Available",
                                 "Double Fault",
                                 "???",
                                 "Invalid TSS",
                                 "Segment Not Present",
                                 "Stack-Segment Fault",
                                 "General Protection Fault",
                                 "Page Fault",
                                 "???",
                                 "x87 Exception",
                                 "Alignment Check",
                                 "Machine Check",
                                 "SIMD Exception",
                                 "Virtualisation",
                                 "???",
                                 "???",
                                 "???",
                                 "???",
                                 "???",
                                 "???",
                                 "???",
                                 "???",
                                 "???",
                                 "Security"};

void idt_load() {
    asm volatile("lidt %0" : : "m"(idtr));
    asm volatile("sti");
}

static void encode_idt_entry(uint8_t vector, void *handler, uint8_t flags) {
    uint64_t ptr = (uint64_t)handler;

    idt[vector].offset_low16 = (uint16_t)ptr;
    idt[vector].selector = 0x28;
    idt[vector].ist = 0;
    idt[vector].flags = flags;
    idt[vector].offset_mid16 = (uint16_t)(ptr >> 16);
    idt[vector].offset_high32 = (uint32_t)(ptr >> 32);
    idt[vector].reserved = 0;
}

static void exception_handler(uint8_t vector, interrupt_frame_t *state) {
    printf("%s\nEXCEPTION RECIVED: %s on core #%d%s\n", cRED,
           exception_names[vector], this_cpu()->cpu_number, cNONE);

    printf("  RAX=%x  RBX=%x\n"
           "  RCX=%x  RDX=%x\n"
           "  RSI=%x  RDI=%x\n"
           "  RBP=%x  RSP=%x\n"
           "  R08=%x  R09=%x\n"
           "  R10=%x  R11=%x\n"
           "  R12=%x  R13=%x\n"
           "  R14=%x  R15=%x\n"
           "  RIP=%x  RFLAGS=%x\n"
           "  CS=%x SS=%x ES=%x DS=%x\n"
           "  ERR=%x",
           state->rax, state->rbx, state->rcx, state->rdx, state->rsi,
           state->rdi, state->rbp, state->rsp, state->r8, state->r9, state->r10,
           state->r11, state->r12, state->r13, state->r14, state->r15,
           state->rip, state->rflags, state->cs, state->ss, state->es,
           state->ds, state->err);

    asm volatile("cli; hlt");
}

static void isr_generic(uint8_t vector, interrupt_frame_t *state) {
    (void)state;
    printf("Something, %d\n", vector);
}

void idt_init() {
    idtr = (idtptr_t){.limit = sizeof(idt) - 1, .base = (uint64_t)idt};

    for (uint64_t i = 0; i < 32; i++) {
        encode_idt_entry(i, isr_stubs[i], 0x8e);
        isr[i] = exception_handler;
    }
    for (uint64_t i = 32; i < 256; i++) {
        encode_idt_entry(i, isr_stubs[i], 0x8e);
        isr[i] = isr_generic;
    }
    idt_load();
    printf("IDT initialized.\n");
}

void idt_set_ist(uint8_t vector, uint8_t ist) { idt[vector].ist = ist; }
