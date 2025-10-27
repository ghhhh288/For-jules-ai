#ifndef STRING_H
#define STRING_H

#include "types.h"

void* memset(void* dest, int val, size_t len);
void* memcpy(void* dest, const void* src, size_t len);
int memcmp(const void* s1, const void* s2, size_t n);
size_t strlen(const char* str);
char* strcpy(char* dest, const char* src);
int strcmp(const char* s1, const char* s2);

#endif
