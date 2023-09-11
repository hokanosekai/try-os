#ifndef __IRQ_H__
#define __IRQ_H__

#include <arch/isr.h>

typedef void (*IRQ_Handler)(Registers* regs);

void IRQ_init();
void IRQ_registerHandler(uint8_t irq, IRQ_Handler handler);

#endif