#ifndef __DRIVER_KEYBOARD_H__
#define __DRIVER_KEYBOARD_H__

#include <libc/stdbool.h>
#include <libc/stdint.h>

void keyboard_get_key(uint8_t* scanCodeOut, uint8_t* asciiCodeOut);

#endif