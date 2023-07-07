#include "dev/apic/lapic.h"
#include "cpu/cpu.h"
#include "cpu/smp.h"
#include "dev/apic/lapic.h"
#include "dev/pit.h"
#include "lib/lock.h"
#include "lib/stdio.h"
#include "lib/vector.h"
#include "memory/vmm.h"
#include <limine.h>

spinlock_t lapic_lock;

static inline uint64_t lapic_base() {
    return (rdmsr(0x1b) & 0xfffff000) + HHDM_OFFSET;
}

uint32_t lapic_read(uint32_t reg) {
    return *((volatile uint32_t *)(lapic_base() + reg));
}

void lapic_write(uint32_t reg, uint32_t data) {
    *((volatile uint32_t *)(lapic_base() + reg)) = data;
}

void lapic_init() {
    // printf("%x\n",lapic_base());
    lapic_write(LAPIC_SPURIOUS_IVR,
                lapic_read(LAPIC_SPURIOUS_IVR) | (1 << 8) | 0xff);
    lapic_calibrate();
}

void lapic_eoi() { lapic_write(LAPIC_EOI, 0x00); }

void lapic_ipi(uint32_t lapic_id, uint8_t vector) {
    lapic_write(LAPIC_ICR1, lapic_id << 24);
    lapic_write(LAPIC_ICR0, vector);
}

uint32_t lapic_id() { return lapic_read(LAPIC_APIC_ID); }

void lapic_stop() {
    lapic_write(LAPIC_TIMER_INIT_COUNT, 0);
    lapic_write(LAPIC_LVT_TIMER_REG, 1 << 16);
}

void lapic_timer_oneshot(uint64_t ms, uint8_t vector) {
    asm volatile("cli");

    lapic_stop();
    uint64_t ticks = ms * (this_cpu()->lapic_freq / 1000);

    lapic_write(LAPIC_LVT_TIMER_REG, vector);
    lapic_write(LAPIC_TIMER_DIVIDE_CFG_REG, 0);
    lapic_write(LAPIC_TIMER_INIT_COUNT, ticks);

    asm volatile("sti");
}

void lapic_calibrate() {
    spinlock_acquire(&lapic_lock);
    lapic_stop();
    pit_set_reload_value(0xffff);
    uint16_t tick_first = pit_get_current_count();

    lapic_write(LAPIC_TIMER_INIT_COUNT, 0xfffff);
    while (lapic_read(LAPIC_TIMER_COUNTER)) {
    }

    uint16_t tick_last = pit_get_current_count();

    uint64_t tick_total = tick_first - tick_last;

    uint64_t freq = 0xfffff / tick_total * OSCILATOR_FREQ;
    // printf("%d freq",freq);

    this_cpu()->lapic_freq = freq;

    lapic_stop();

    spinlock_release(&lapic_lock);
}
