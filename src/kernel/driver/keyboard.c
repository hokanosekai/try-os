#include "keyboard.h"
#include "x86.h"
#include "../lib/stdio.h"
#include <stdint.h>

void keyboard_get_key(uint8_t* scanCodeOut, uint8_t* asciiCodeOut) {
  uint8_t status;
  uint8_t scanCode;
  uint8_t asciiCode;

  // Wait for the keyboard buffer to be ready
  status = x86_inb(0x64);
  if ((status & 0x01) == 0) {
    return;
  }

  // Read the scan code
  scanCode = x86_inb(0x60);

  printf("scanCode: %c\n", scanCode);

  if (scanCode < 0x80) {
    asciiCode = scanCode;
  } else {
    asciiCode = 0;
  }

  *scanCodeOut = scanCode;
  *asciiCodeOut = asciiCode;

    // Check if the caps lock key is pressed
    
}