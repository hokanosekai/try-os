#include "font.h"
#include "symbols.h"
#include <stddef.h>

int8_t font_a[FONT_HEIGHT] = {A};
int8_t font_b[FONT_HEIGHT] = {B};
int8_t font_c[FONT_HEIGHT] = {C};
int8_t font_d[FONT_HEIGHT] = {D};
int8_t font_e[FONT_HEIGHT] = {E};
int8_t font_f[FONT_HEIGHT] = {F};

void font_get(int8_t** font, char c) {
  switch (c) {
    case 'A':
      *font = font_a;
      break;
    case 'B':
      *font = font_b;
      break;
    case 'C':
      *font = font_c;
      break;
    case 'D':
      *font = font_d;
      break;
    case 'E':
      *font = font_e;
      break;
    case 'F':
      *font = font_f;
      break;
    default:
      *font = NULL;
      break;
  }
}