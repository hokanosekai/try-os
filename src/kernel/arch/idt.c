#include <arch/idt.h>
#include <sys/binary.h>

typedef struct {
  uint16_t base_low;
  uint16_t segment;
  uint8_t reserved;
  uint8_t flags;
  uint16_t base_high;
} __attribute__((packed)) IDT_Entry;

typedef struct {
  uint16_t limit;
  IDT_Entry* pointer;
} __attribute__((packed)) IDT_Descriptor;

IDT_Entry g_IDT[256];

IDT_Descriptor g_IDTDescriptor = {
  sizeof(g_IDT) - 1,
  g_IDT
};

void __attribute__((cdecl)) IDT_load(IDT_Descriptor* descriptor);

void IDT_init() {
  IDT_load(&g_IDTDescriptor);
}

void IDT_disableGate(uint8_t gate) {
  FLAG_UNSET(g_IDT[gate].flags, IDT_FLAG_PRESENT);
}

void IDT_enableGate(uint8_t gate) {
  FLAG_SET(g_IDT[gate].flags, IDT_FLAG_PRESENT);
}

void IDT_setGate(uint8_t gate, void* base, uint16_t segment, uint8_t flags) {
  g_IDT[gate].base_low  = ((uint32_t)base) & 0xFFFF;
  g_IDT[gate].base_high = ((uint32_t)base >> 16) & 0xFFFF;
  g_IDT[gate].segment   = segment;
  g_IDT[gate].reserved  = 0;
  g_IDT[gate].flags     = flags;
}