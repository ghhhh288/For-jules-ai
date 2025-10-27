#include "../include/types.h"
#include "../include/vga.h"
#include "../include/gdt.h"
#include "../include/idt.h"
#include "../include/timer.h"
#include "../include/keyboard.h"
#include "../include/paging.h"
#include "../include/process.h"
#include "../include/string.h"

void print_logo(void) {
    vga_setcolor(0x0B);
    vga_writestring("  _     _                    _  __                    _ \n");
    vga_writestring(" | |   (_)_ __  _   ___  __ | |/ /___ _ __ _ __   ___| |\n");
    vga_writestring(" | |   | | '_ \\| | | \\ \\/ / | ' // _ \\ '__| '_ \\ / _ \\ |\n");
    vga_writestring(" | |___| | | | | |_| |>  <  | . \\  __/ |  | | | |  __/ |\n");
    vga_writestring(" |_____|_|_| |_|\\__,_/_/\\_\\ |_|\\_\\___|_|  |_| |_|\\___|_|\n");
    vga_writestring("\n");
    vga_setcolor(0x07);
}

void print_info(void) {
    vga_writestring("Linux Kernel v1.0.0\n");
    vga_writestring("Built: 2025\n");
    vga_writestring("Architecture: x86 (32-bit)\n");
    vga_writestring("\n");
}

void print_subsystems(void) {
    vga_setcolor(0x0A);
    vga_writestring("[OK] ");
    vga_setcolor(0x07);
    vga_writestring("GDT initialized\n");
    
    vga_setcolor(0x0A);
    vga_writestring("[OK] ");
    vga_setcolor(0x07);
    vga_writestring("IDT initialized\n");
    
    vga_setcolor(0x0A);
    vga_writestring("[OK] ");
    vga_setcolor(0x07);
    vga_writestring("Timer initialized (100Hz)\n");
    
    vga_setcolor(0x0A);
    vga_writestring("[OK] ");
    vga_setcolor(0x07);
    vga_writestring("Keyboard initialized\n");
    
    vga_setcolor(0x0A);
    vga_writestring("[OK] ");
    vga_setcolor(0x07);
    vga_writestring("Paging initialized\n");
    
    vga_setcolor(0x0A);
    vga_writestring("[OK] ");
    vga_setcolor(0x07);
    vga_writestring("Tasking initialized\n");
    
    vga_writestring("\n");
}

void shell(void) {
    vga_setcolor(0x0E);
    vga_writestring("kernel> ");
    vga_setcolor(0x07);
    
    char buffer[256];
    int pos = 0;
    
    while (1) {
        char c = keyboard_getchar();
        
        if (c == '\n') {
            vga_putchar('\n');
            buffer[pos] = '\0';
            
            if (strcmp(buffer, "help") == 0) {
                vga_writestring("Available commands:\n");
                vga_writestring("  help    - Show this help message\n");
                vga_writestring("  clear   - Clear the screen\n");
                vga_writestring("  info    - Show system information\n");
                vga_writestring("  uptime  - Show system uptime\n");
                vga_writestring("  reboot  - Reboot the system\n");
            } else if (strcmp(buffer, "clear") == 0) {
                vga_clear();
            } else if (strcmp(buffer, "info") == 0) {
                print_info();
            } else if (strcmp(buffer, "uptime") == 0) {
                vga_writestring("System uptime: ");
                uint32_t ticks = timer_ticks();
                uint32_t seconds = ticks / 100;
                uint32_t minutes = seconds / 60;
                uint32_t hours = minutes / 60;
                
                char time_str[32];
                time_str[0] = '0' + (hours / 10);
                time_str[1] = '0' + (hours % 10);
                time_str[2] = ':';
                time_str[3] = '0' + ((minutes % 60) / 10);
                time_str[4] = '0' + ((minutes % 60) % 10);
                time_str[5] = ':';
                time_str[6] = '0' + ((seconds % 60) / 10);
                time_str[7] = '0' + ((seconds % 60) % 10);
                time_str[8] = '\0';
                
                vga_writestring(time_str);
                vga_writestring("\n");
            } else if (strcmp(buffer, "reboot") == 0) {
                vga_writestring("Rebooting...\n");
                uint8_t temp;
                do {
                    __asm__ volatile("inb $0x64, %0" : "=a"(temp));
                } while (temp & 0x02);
                __asm__ volatile("outb %0, $0x64" : : "a"((uint8_t)0xFE));
            } else if (pos > 0) {
                vga_writestring("Unknown command: ");
                vga_writestring(buffer);
                vga_writestring("\n");
            }
            
            pos = 0;
            vga_setcolor(0x0E);
            vga_writestring("kernel> ");
            vga_setcolor(0x07);
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                vga_putchar('\b');
                vga_putchar(' ');
                vga_putchar('\b');
            }
        } else if (pos < 255) {
            buffer[pos++] = c;
            vga_putchar(c);
        }
    }
}

void kernel_main(uint32_t magic, uint32_t addr) {
    (void)magic;
    (void)addr;
    
    vga_init();
    vga_clear();
    
    print_logo();
    print_info();
    
    gdt_init();
    idt_init();
    timer_init(100);
    keyboard_init();
    paging_init();
    tasking_init();
    
    print_subsystems();
    
    vga_setcolor(0x0A);
    vga_writestring("Kernel initialization complete!\n");
    vga_setcolor(0x07);
    vga_writestring("Type 'help' for available commands.\n\n");
    
    shell();
    
    for(;;);
}
