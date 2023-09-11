#ifndef __ISR_H__
#define __ISR_H__

#include <libc/stdint.h>

typedef struct {
  uint32_t ds;                                      // Data segment selector.
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // Pushed by pusha.
  uint32_t int_no, err_code;                        // Interrupt number and error code (if applicable).
  uint32_t eip, cs, eflags, esp, ss;                // Pushed by the processor automatically.
} __attribute__((packed)) Registers;

typedef void (*ISR_Handler)(Registers* regs);

void ISR_init();
void ISR_registerHandler(uint8_t isr, ISR_Handler handler);

#endif