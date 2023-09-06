#include "window.h"
#include "stdio.h"
#include "../driver/vbe.h"
#include "memdefs.h"
#include "colors.h"
#include "math.h"
#include "minmax.h"
#include "string.h"
#include "font.h"

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

void window_draw_circle(window_t* window,
                        const int x,
                        const int y,
                        const int radius,
                        const uint32_t color) {
  int x1 = 0;
  int y1 = radius;
  int delta = 1 - 2 * radius;

  int error = 0;

  while (y1 >= 0) {
    window_draw_pixel(window, x + x1, y + y1, color);
    window_draw_pixel(window, x + x1, y - y1, color);
    window_draw_pixel(window, x - x1, y + y1, color);
    window_draw_pixel(window, x - x1, y - y1, color);

    error = 2 * (delta + y1) - 1;
    if (delta < 0 && error <= 0) {
      delta += 2 * ++x1 + 1;
      continue;
    }

    error = 2 * (delta - x1) - 1;
    if (delta > 0 && error > 0) {
      delta += 1 - 2 * --y1;
      continue;
    }

    x1++;
    delta += 2 * (x1 - y1);
    y1--;
  }
}

void window_draw_circle_fill(window_t* window,
                             const int x,
                             const int y,
                             const int radius,
                             const uint32_t color) {
  int x1 = 0;
  int y1 = radius;
  int delta = 1 - 2 * radius;

  int error = 0;

  while (y1 >= 0) {
    window_draw_line(window, x + x1, y + y1, x + x1, y - y1, color);
    window_draw_line(window, x - x1, y + y1, x - x1, y - y1, color);

    error = 2 * (delta + y1) - 1;
    if (delta < 0 && error <= 0) {
      delta += 2 * ++x1 + 1;
      continue;
    }

    error = 2 * (delta - x1) - 1;
    if (delta > 0 && error > 0) {
      delta += 1 - 2 * --y1;
      continue;
    }

    x1++;
    delta += 2 * (x1 - y1);
    y1--;
  }
}

void window_draw_triangle(window_t* window,
                          const int x1,
                          const int y1,
                          const int x2,
                          const int y2,
                          const int x3,
                          const int y3,
                          const uint32_t color) {
  window_draw_line(window, x1, y1, x2, y2, color);
  window_draw_line(window, x2, y2, x3, y3, color);
  window_draw_line(window, x3, y3, x1, y1, color);
}

/**
 * TODO : Fix this function
 */
void window_draw_triangle_fill(window_t* window,
                               const int x1,
                               const int y1,
                               const int x2,
                               const int y2,
                               const int x3,
                               const int y3,
                               const uint32_t color) {
  int minX = min(x1, min(x2, x3));
  int minY = min(y1, min(y2, y3));
  int maxX = max(x1, max(x2, x3));
  int maxY = max(y1, max(y2, y3));

  for (int y = minY; y <= maxY; y++) {
    for (int x = minX; x <= maxX; x++) {
      int w1 = (x1 * (y2 - y3) + (y - y1) * (x2 - x3) + x3 * y1 - x2 * y1) / 2;
      int w2 = (x2 * (y3 - y1) + (y - y2) * (x3 - x1) + x1 * y2 - x3 * y2) / 2;
      int w3 = (x3 * (y1 - y2) + (y - y3) * (x1 - x2) + x2 * y3 - x1 * y3) / 2;

      if (w1 >= 0 && w2 >= 0 && w3 >= 0) {
        window_draw_pixel(window, x, y, color);
      }
    }
  }
}

void window_draw_char(window_t* window,
                      const int x,
                      const int y,
                      const char c,
                      const uint32_t color) {
  int8_t* font;
  font_get(&font, c);

  printf("Drawing char %c\n", c);

  for (int i = 0; i < 10; i++) {
    int line = font[i];
    printf("Line %x\n", line);
    for (int j = 7; j >= 0; j--) {
      int bit = (line >> j) & 1;
      //printf("Bit %d\n", bit);
      if (bit) {
        window_draw_pixel(window, x + (7 - j), y + i, color);
      }
    }
  }
}

void window_draw_string(window_t* window,
                        const int x,
                        const int y,
                        const char* str,
                        const uint32_t color) {
  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    window_draw_char(window, x + i * 10, y, str[i], color);
  }
}