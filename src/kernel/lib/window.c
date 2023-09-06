#include "window.h"
#include "stdio.h"
#include "../driver/vbe.h"
#include "memdefs.h"
#include "colors.h"
#include <stddef.h>
#include <stdbool.h>

bool window_init(window_t* window, const int width, const int height, const int bpp) {
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

void window_draw_pixel(window_t *window, const int x, const int y, const uint32_t color) {
  uint32_t* fb = (uint32_t*)window->mode_info->framebuffer;
  fb[y * window->mode_info->width + x] = color;
}