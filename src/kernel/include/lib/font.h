#ifndef __FONT_H__
#define __FONT_H__

#include <libc/stdint.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 10

void font_get(int8_t** font, char c);
void font_get_alpha(int8_t** font, char c);
void font_get_digit(int8_t** font, char c);

#endif