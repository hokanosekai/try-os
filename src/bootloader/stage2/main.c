#include <stdint.h>
#include "stdio.h"
#include "x86.h"
#include "memdefs.h"
#include "memory.h"
#include "vbe.h"

#define COLOR(r,g,b) ((b) | (g << 8) | (r << 16))

void __attribute__((cdecl)) start(uint16_t bootdrive) {
  cls();

/*
  const int width = 1024;
  const int height = 768;
  const int bpp = 32;
  uint16_t pickedMode = 0x0000; // 0xFFFF = no mode, 0x0000 = text mode, 0x0100 = 320x200x8, 0x0101 = 320x200x16, 0x0102 = 320x200x24, 0x0103 = 320x200x32, etc.

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

    if (pickedMode != 0xFFFF && VBE_SetMode(pickedMode)) {
      uint32_t* fb = (uint32_t*)modeInfo->framebuffer;
      int w = modeInfo->width;
      int h = modeInfo->height;
      for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++)
          fb[y * modeInfo->pitch / 4 + x] = COLOR(128, 58, 175);
      }
    }

    printf("VBE info:\n");
    printf("  Signature: %c%c%c%c\n", info->signature[0], info->signature[1], info->signature[2], info->signature[3]);
    printf("  Version: %x\n", info->version);
    printf("  OEM String: %x\n", info->oem_string_ptr);
    printf("  Capabilities: %x\n", info->capabilities);
    printf("  Video Mode Ptr: %x\n", info->video_mode_ptr);
    printf("  Total Memory: %x\n", info->total_memory);
  } else {
    printf("Can't get controller info\n");
  }*/

  printf("Hello, world!\n");

  for (;;);
}