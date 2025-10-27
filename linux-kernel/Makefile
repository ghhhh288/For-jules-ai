AS = nasm
CC = gcc
LD = ld

ASFLAGS = -f elf32
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -c
LDFLAGS = -m elf_i386 -T linker.ld

SOURCES_ASM = boot/boot.asm
SOURCES_C = kernel/kernel.c \
            lib/string.c \
            drivers/vga.c \
            drivers/timer.c \
            drivers/keyboard.c \
            arch/x86/gdt.c \
            arch/x86/idt.c \
            mm/kmalloc.c \
            mm/paging.c \
            kernel/process.c

OBJECTS_ASM = $(SOURCES_ASM:.asm=.o)
OBJECTS_C = $(SOURCES_C:.c=.o)
OBJECTS = $(OBJECTS_ASM) $(OBJECTS_C)

KERNEL = kernel.bin
ISO = kernel.iso

.PHONY: all clean run iso

all: $(KERNEL)

$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

iso: $(KERNEL)
	@mkdir -p isodir/boot/grub
	@cp $(KERNEL) isodir/boot/$(KERNEL)
	@echo 'menuentry "Linux Kernel" {' > isodir/boot/grub/grub.cfg
	@echo '    multiboot /boot/$(KERNEL)' >> isodir/boot/grub/grub.cfg
	@echo '}' >> isodir/boot/grub/grub.cfg
	@grub-mkrescue -o $(ISO) isodir 2>/dev/null || echo "grub-mkrescue not found. Install grub-pc-bin and xorriso."

run: $(KERNEL)
	qemu-system-i386 -kernel $(KERNEL)

run-iso: iso
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -f $(OBJECTS) $(KERNEL) $(ISO)
	rm -rf isodir

# Dependencies
boot/boot.o: boot/boot.asm
kernel/kernel.o: kernel/kernel.c include/*.h
lib/string.o: lib/string.c include/string.h include/types.h
drivers/vga.o: drivers/vga.c include/vga.h include/string.h include/types.h
drivers/timer.o: drivers/timer.c include/timer.h include/idt.h include/types.h
drivers/keyboard.o: drivers/keyboard.c include/keyboard.h include/idt.h include/types.h
arch/x86/gdt.o: arch/x86/gdt.c include/gdt.h include/string.h include/types.h
arch/x86/idt.o: arch/x86/idt.c include/idt.h include/string.h include/vga.h include/types.h
mm/kmalloc.o: mm/kmalloc.c include/kmalloc.h include/paging.h include/types.h
mm/paging.o: mm/paging.c include/paging.h include/kmalloc.h include/string.h include/vga.h include/idt.h include/types.h
kernel/process.o: kernel/process.c include/process.h include/kmalloc.h include/string.h include/paging.h include/types.h
