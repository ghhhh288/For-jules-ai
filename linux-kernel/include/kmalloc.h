#ifndef KMALLOC_H
#define KMALLOC_H

#include "types.h"

void* kmalloc(uint32_t size);
void* kmalloc_a(uint32_t size);
void* kmalloc_p(uint32_t size, uint32_t *phys);
void* kmalloc_ap(uint32_t size, uint32_t *phys);
void kfree(void* ptr);

#endif
