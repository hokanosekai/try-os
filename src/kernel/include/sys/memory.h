#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <libc/stdint.h>

#define SEG(seg_off) (seg_off>>16)
#define OFF(seg_off) (seg_off&0xFFFF)
#define SEG_OFF_2_LIN(seg_off) ((SEG(seg_off) << 4) + OFF(seg_off))

void* memcpy(void* dest, const void* src, uint16_t count);
void* memset(void* dest, int val, uint16_t count);
int memcmp(const void* ptr1, const void* ptr2, uint16_t count);

#endif