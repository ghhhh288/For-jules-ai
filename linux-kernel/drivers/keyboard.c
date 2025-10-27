#include "../include/keyboard.h"
#include "../include/idt.h"

static char keyboard_buffer[256];
static int buffer_pos = 0;

static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static void keyboard_callback(struct registers *regs) {
    (void)regs;
    uint8_t scancode;
    __asm__ volatile("inb %1, %0" : "=a"(scancode) : "Nd"((uint16_t)0x60));
    
    if (scancode & 0x80) {
        // Key release
    } else {
        // Key press
        if (scancode < sizeof(scancode_to_ascii)) {
            char c = scancode_to_ascii[scancode];
            if (c != 0 && buffer_pos < 255) {
                keyboard_buffer[buffer_pos++] = c;
            }
        }
    }
}

void keyboard_init(void) {
    register_interrupt_handler(33, &keyboard_callback);
}

char keyboard_getchar(void) {
    while (buffer_pos == 0);
    char c = keyboard_buffer[0];
    for (int i = 0; i < buffer_pos - 1; i++) {
        keyboard_buffer[i] = keyboard_buffer[i + 1];
    }
    buffer_pos--;
    return c;
}
