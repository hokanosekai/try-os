#pragma once

#include "../driver/vbe.h"

typedef struct {
  vbe_info_t* info;
  vbe_mode_info_t* mode_info;
} window_t;

bool window_init(window_t* window,
                 const int width,
                 const int height,
                 const int bpp);

void window_clear(window_t* window, const uint32_t color);

void window_draw_pixel(window_t* window,
                       const int x,
                       const int y,
                       const uint32_t color);