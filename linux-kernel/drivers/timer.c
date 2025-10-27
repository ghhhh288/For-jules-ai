#include "../include/timer.h"
#include "../include/idt.h"

static uint32_t tick = 0;

static void timer_callback(struct registers *regs) {
    (void)regs;
    tick++;
}

void timer_init(uint32_t frequency) {
    register_interrupt_handler(32, &timer_callback);
    
    uint32_t divisor = 1193180 / frequency;
    
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x36), "Nd"((uint16_t)0x43));
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)(divisor & 0xFF)), "Nd"((uint16_t)0x40));
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)((divisor >> 8) & 0xFF)), "Nd"((uint16_t)0x40));
}

uint32_t timer_ticks(void) {
    return tick;
}

void timer_wait(uint32_t ticks) {
    uint32_t eticks = tick + ticks;
    while (tick < eticks);
}
