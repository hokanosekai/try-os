#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <libc/stdint.h>

void* memcpy(void* dest, const void* src, uint16_t count);
void* memset(void* dest, int val, uint16_t count);
int memcmp(const void* ptr1, const void* ptr2, uint16_t count);

void* seg2lin(void* addr);

#endif