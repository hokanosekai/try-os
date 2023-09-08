#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Here we define the keyboard driver functions
 * This will be used to get all the keyboard inputs
 */
void keyboard_get_key(uint8_t* scanCodeOut, uint8_t* asciiCodeOut);