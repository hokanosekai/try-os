#include <drivers/x86.h>
#include <io/io.h>

void outb(uint16_t port, uint8_t value) {
  x86_outb(port, value);
}

uint8_t inb(uint16_t port) {
  return x86_inb(port);
}