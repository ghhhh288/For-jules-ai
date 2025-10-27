# Linux Kernel

A full-featured x86 operating system kernel built from scratch with essential subsystems.

## Features

### Core Components
- **Bootloader**: Multiboot-compliant bootloader for GRUB
- **Memory Management**: 
  - Paging with 4KB pages
  - Virtual memory support
  - Kernel heap allocator (kmalloc)
  - Page fault handling
- **Process Management**:
  - Task scheduling
  - Process creation (fork)
  - Context switching
  - Process states (running, ready, blocked, terminated)
- **Interrupt Handling**:
  - Global Descriptor Table (GDT)
  - Interrupt Descriptor Table (IDT)
  - ISR and IRQ handlers
  - PIC remapping

### Drivers
- **VGA Text Mode Driver**: 80x25 color text display with scrolling
- **Timer Driver**: Programmable Interval Timer (PIT) at 100Hz
- **Keyboard Driver**: PS/2 keyboard input with scancode translation

### User Interface
- Interactive shell with commands:
  - `help` - Display available commands
  - `clear` - Clear the screen
  - `info` - Show system information
  - `uptime` - Display system uptime
  - `reboot` - Reboot the system

## Architecture

```
linux-kernel/
├── boot/           # Bootloader and assembly entry point
├── kernel/         # Core kernel code
├── arch/x86/       # x86-specific code (GDT, IDT)
├── mm/             # Memory management
├── drivers/        # Device drivers
├── include/        # Header files
├── lib/            # Utility libraries
├── linker.ld       # Linker script
└── Makefile        # Build system
```

## Building

### Prerequisites
- GCC cross-compiler for i686-elf or 32-bit support
- NASM assembler
- GNU Make
- QEMU (for testing)
- GRUB (for ISO creation)

### Compilation

```bash
# Build the kernel
make

# Create bootable ISO
make iso

# Run in QEMU
make run

# Run ISO in QEMU
make run-iso

# Clean build artifacts
make clean
```

## Running

### With QEMU (Direct Kernel Boot)
```bash
qemu-system-i386 -kernel kernel.bin
```

### With QEMU (ISO Boot)
```bash
qemu-system-i386 -cdrom kernel.iso
```

### On Real Hardware
1. Create a bootable USB with the ISO:
   ```bash
   dd if=kernel.iso of=/dev/sdX bs=4M
   ```
2. Boot from the USB drive

## Technical Details

### Memory Layout
- Kernel loaded at: `0x00100000` (1MB)
- VGA text buffer: `0xB8000`
- Page size: 4KB
- Identity mapping: First 4MB

### Interrupt Vector Table
- ISR 0-31: CPU exceptions
- IRQ 32-47: Hardware interrupts
  - IRQ 0 (32): Timer
  - IRQ 1 (33): Keyboard
  - IRQ 14 (46): Page fault

### GDT Segments
0. Null segment
1. Kernel code segment (0x08)
2. Kernel data segment (0x10)
3. User code segment (0x18)
4. User data segment (0x20)

## Development

### Adding New Features
1. Create header file in `include/`
2. Implement in appropriate directory
3. Update Makefile dependencies
4. Rebuild with `make`

### Debugging
```bash
# Run with GDB support
qemu-system-i386 -kernel kernel.bin -s -S

# In another terminal
gdb kernel.bin
(gdb) target remote localhost:1234
(gdb) continue
```

## Limitations

- 32-bit x86 only
- No filesystem support
- Simple round-robin scheduler
- Basic memory allocator (no free)
- Limited driver support

## Future Enhancements

- [ ] Filesystem (ext2/FAT32)
- [ ] System calls
- [ ] User mode programs
- [ ] Network stack
- [ ] Advanced scheduler
- [ ] SMP support
- [ ] 64-bit support

## License

This is an educational kernel implementation. Feel free to use and modify.

## References

- [OSDev Wiki](https://wiki.osdev.org/)
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
