#include "window.h"
#include "stdio.h"
#include "../driver/vbe.h"
#include "memdefs.h"
#include "colors.h"
#include "math.h"

#include <stddef.h>
#include <stdbool.h>

bool window_init(window_t* window, 
                 const int width, 
                 const int height, 
                 const int bpp) {
  uint16_t pickedMode = 0xFFFF;

  vbe_info_t* info = (vbe_info_t*)MEMORY_VESA_INFO;
  vbe_mode_info_t* modeInfo = (vbe_mode_info_t*)MEMORY_MODE_INFO;

  if (VBE_GetControllerInfo(info)) {
    uint16_t* mode = (uint16_t*)info->video_mode_ptr;
    for (int i = 0; mode[i] != 0xFFFF; i++) {
      if (!VBE_GetModeInfo(mode[i], modeInfo)) {
        printf("Can't get mode info %x\n", mode[i]);
        continue;
      }
      bool hasFB = (modeInfo->attributes & 0x90) == 0x90;
      if (hasFB && modeInfo->width == width && modeInfo->height == height && modeInfo->bpp == bpp) {
        pickedMode = mode[i];
        break;
      }
    }
  }

  if (pickedMode == 0xFFFF) {
    printf("Can't find suitable mode\n");
    return false;
  }

  if (!VBE_SetMode(pickedMode)) {
    printf("Can't set mode %x\n", pickedMode);
    return false;
  }

  window->info = info;
  window->mode_info = modeInfo;

  return true;
}

void window_clear(window_t *window, const uint32_t color) {
  uint32_t* fb = (uint32_t*)window->mode_info->framebuffer;
  for (int i = 0; i < window->mode_info->width * window->mode_info->height; i++) {
    fb[i] = color;
  }
}

void window_draw_pixel(window_t *window, 
                       const int x, 
                       const int y, 
                       const uint32_t color) {
  uint32_t* fb = (uint32_t*)window->mode_info->framebuffer;
  fb[y * window->mode_info->width + x] = color;
}

void window_draw_line(window_t* window,
                      const int x1, 
                      const int y1, 
                      const int x2, 
                      const int y2, 
                      const uint32_t color) {
  int dx = x2 - x1;
  int dy = y2 - y1;

  int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

  if (dx < 0) dx1 = -1; else if (dx > 0) dx1 = 1;
  if (dy < 0) dy1 = -1; else if (dy > 0) dy1 = 1;
  if (dx < 0) dx2 = -1; else if (dx > 0) dx2 = 1;

  int longest = abs(dx);
  int shortest = abs(dy);

  if (!(longest > shortest)) {
    longest = abs(dy);
    shortest = abs(dx);
    if (dy < 0) dy2 = -1; else if (dy > 0) dy2 = 1;
    dx2 = 0;
  }

  int x = x1;
  int y = y1;

  int numerator = longest >> 1;
  for (int i = 0; i <= longest; i++) {
    window_draw_pixel(window, x, y, color);
    numerator += shortest;
    if (!(numerator < longest)) {
      numerator -= longest;
      x += dx1;
      y += dy1;
    } else {
      x += dx2;
      y += dy2;
    }
  }
}

void window_draw_rect(window_t* window, 
                      const int x, 
                      const int y, 
                      const int width, 
                      const int height, 
                      const uint32_t color) {
  window_draw_line(window, x, y, x + width, y, color);
  window_draw_line(window, x + width, y, x + width, y + height, color);
  window_draw_line(window, x + width, y + height, x, y + height, color);
  window_draw_line(window, x, y + height, x, y, color);
}

void window_draw_rect_fill(window_t* window, 
                           const int x, 
                           const int y, 
                           const int width, 
                           const int height, 
                           const uint32_t color) {
  uint32_t* fb = (uint32_t*)window->mode_info->framebuffer;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++)
      fb[(y + i) * window->mode_info->width + x + j] = color;
  }
}