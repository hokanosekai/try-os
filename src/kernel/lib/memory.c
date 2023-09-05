#include "memory.h"
#include "stdio.h"

void* memcpy(void* dest, const void* src, uint16_t count) {
  uint8_t* u8Dest = (uint8_t*)dest;
  const uint8_t* u8Src = (const uint8_t*)src;

  for (uint16_t i = 0; i < count; i++) {
    u8Dest[i] = u8Src[i];
  }

  return dest;
}

void* memset(void* dest, int val, uint16_t count) {
  uint8_t* u8Dest = (uint8_t*)dest;

  for (uint16_t i = 0; i < count; i++) {
    u8Dest[i] = (uint8_t)val;
  }

  return dest;
}

int memcmp(const void* ptr1, const void* ptr2, uint16_t count) {
  const uint8_t* u8Ptr1 = (const uint8_t*)ptr1;
  const uint8_t* u8Ptr2 = (const uint8_t*)ptr2;

  for (uint16_t i = 0; i < count; i++) {
    printf("%d %d\n", u8Ptr1[i], u8Ptr2[i]);
    if (u8Ptr1[i] != u8Ptr2[i]) {
      return 1;
    }
  }

  return 0;
}