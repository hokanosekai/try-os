#ifndef __GDT_H__
#define __GDT_H__

#include <libc/stdint.h>

#define GDT_CODE_SEGMENT 0x08
#define GDT_DATA_SEGMENT 0x10

typedef enum {
  GDT_ACCESS_CODE_READABLE          = 0x02,
  GDT_ACCESS_DATA_WRITEABLE         = 0x02,

  GDT_ACCESS_CODE_CONFORMING        = 0x04,
  GDT_ACCESS_DATA_DIRECTION_NORMAL  = 0x00,
  GDT_ACCESS_DATA_DIRECTION_DOWN    = 0x04,

  GDT_ACCESS_DATA_SEGMENT           = 0x10,
  GDT_ACCESS_CODE_SEGMENT           = 0x18,

  GDT_ACCESS_DESCRIPTOR_TSS         = 0x00,

  GDT_ACCESS_RING_0                 = 0x00,
  GDT_ACCESS_RING_1                 = 0x20,
  GDT_ACCESS_RING_2                 = 0x40,
  GDT_ACCESS_RING_3                 = 0x60,

  GDT_ACCESS_PRESENT                = 0x80
} GDT_Access;

typedef enum {
  GDT_FLAG_16_BIT                   = 0x00,
  GDT_FLAG_32_BIT                   = 0x40,
  GDT_FLAG_64_BIT                   = 0x20,

  GDT_FLAG_GRANULARITY_1B           = 0x00,
  GDT_FLAG_GRANULARITY_4KB          = 0x80
} GDT_Flag;

#define GDT_LIMIT_LOW(limit)                (limit & 0xFFFF)
#define GDT_BASE_LOW(base)                  (base & 0xFFFF)
#define GDT_BASE_MIDDLE(base)               ((base >> 16) & 0xFF)
#define GDT_FLAGS_LIMIT_HI(limit, flags)    (((limit >> 16) & 0xF) | (flags & 0xF0))
#define GDT_BASE_HIGH(base)                 ((base >> 24) & 0xFF)

#define GDT_ENTRY(base, limit, access, flags) {                     \
    GDT_LIMIT_LOW(limit),                                           \
    GDT_BASE_LOW(base),                                             \
    GDT_BASE_MIDDLE(base),                                          \
    access,                                                         \
    GDT_FLAGS_LIMIT_HI(limit, flags),                               \
    GDT_BASE_HIGH(base)                                             \
}

int GDT_init();

#endif