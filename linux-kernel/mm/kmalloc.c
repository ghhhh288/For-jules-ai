#include "../include/kmalloc.h"
#include "../include/paging.h"

extern uint32_t end;
uint32_t placement_address = (uint32_t)&end;

static uint32_t kmalloc_int(uint32_t size, int align, uint32_t *phys) {
    if (align == 1 && (placement_address & 0xFFFFF000)) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    
    if (phys) {
        *phys = placement_address;
    }
    
    uint32_t tmp = placement_address;
    placement_address += size;
    return tmp;
}

void* kmalloc(uint32_t size) {
    return (void*)kmalloc_int(size, 0, 0);
}

void* kmalloc_a(uint32_t size) {
    return (void*)kmalloc_int(size, 1, 0);
}

void* kmalloc_p(uint32_t size, uint32_t *phys) {
    return (void*)kmalloc_int(size, 0, phys);
}

void* kmalloc_ap(uint32_t size, uint32_t *phys) {
    return (void*)kmalloc_int(size, 1, phys);
}

void kfree(void* ptr) {
    (void)ptr;
    // Simple allocator doesn't support freeing
}
