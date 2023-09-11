#ifndef __IDT_H__
#define __IDT_H__

#include <libc/stdint.h>

typedef enum {
  IDT_FLAG_GATE_TASK        = 0x05,
  IDT_FLAG_GATE_16BIT_INT   = 0x06,
  IDT_FLAG_GATE_16BIT_TRAP  = 0x07,
  IDT_FLAG_GATE_32BIT_INT   = 0x0E,
  IDT_FLAG_GATE_32BIT_TRAP  = 0x0F,

  IDT_FLAG_RING_0           = (0x00 << 5),
  IDT_FLAG_RING_1           = (0x01 << 5),
  IDT_FLAG_RING_2           = (0x02 << 5),
  IDT_FLAG_RING_3           = (0x03 << 5),

  IDT_FLAG_PRESENT          = 0x80
} IDT_Flags;

void IDT_init();
void IDT_disableGate(uint8_t gate);
void IDT_enableGate(uint8_t gate);
void IDT_setGate(uint8_t gate, void* base, uint16_t segment, uint8_t flags);

#endif