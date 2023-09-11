#include <arch/gdt.h>

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) GDT_Entry;

typedef struct {
    uint16_t limit;
    GDT_Entry* pointer;
} __attribute__((packed)) GDT_Descriptor;

GDT_Entry g_GDT[] = {
  GDT_ENTRY(0, 0, 0, 0), // Null segment
  GDT_ENTRY(
    0, // Base address
    0xFFFFFFFF, // Limit
    GDT_ACCESS_PRESENT | GDT_ACCESS_RING_0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE, // Access
    GDT_FLAG_GRANULARITY_4KB | GDT_FLAG_32_BIT
  ), // Kernel code segment

  GDT_ENTRY(
    0, // Base address
    0xFFFFFFFF, // Limit
    GDT_ACCESS_PRESENT | GDT_ACCESS_RING_0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE, // Access
    GDT_FLAG_GRANULARITY_4KB | GDT_FLAG_32_BIT
  ), // Kernel data segment
};

GDT_Descriptor g_GDTDescriptor = {
  sizeof(g_GDT) - 1,
  g_GDT
};

int __attribute__((cdecl)) GDT_load(GDT_Descriptor* descriptor, uint16_t codeSegment, uint16_t dataSegment);

int GDT_init() {
  return GDT_load(&g_GDTDescriptor, GDT_CODE_SEGMENT, GDT_DATA_SEGMENT);
}