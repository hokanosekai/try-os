#ifndef __IO_H__
#define __IO_H__

#include <libc/stdint.h>

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

#endif