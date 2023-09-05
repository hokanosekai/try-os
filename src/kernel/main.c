#include <stdint.h>
#include "lib/stdio.h"
#include "driver/x86.h"
#include "lib/memdefs.h"
#include "lib/memory.h"
#include "driver/vbe.h"
#include "driver/disk.h"
#include "driver/fat.h"


#define COLOR(r,g,b) ((b) | (g << 8) | (r << 16))

void __attribute__((cdecl)) start(uint16_t drive) {
  cls();

  disk_t disk;
  if (!disk_init(&disk, drive)) {
    printf("Disk init failed\n");
    goto end;
  }

  if (!fat_init(&disk)) {
    printf("FAT init failed\n");
    goto end;
  }

  // Read test.txt file
  char buffer[512];
  fat_file_t* file = fat_open(&disk, "/test.txt");
  uint32_t read;
  read = fat_read(&disk, file, sizeof(buffer), buffer);
  if (read != file->size) {
    printf("File read failed\n");
    goto end;
  }

  fat_close(file);
  printf("%s\n", buffer);

  const int width = 1024;
  const int height = 768;
  const int bpp = 32;
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

      printf("Mode: %x\n", mode[i]);
      printf("  Width: %d\n  Height: %d\n  BPP: %d\n", modeInfo->width, modeInfo->height, modeInfo->bpp);

      if (hasFB && modeInfo->width == width && modeInfo->height == height && modeInfo->bpp == bpp) {
        pickedMode = mode[i];
        break;
      }
    }

    printf("Picked mode: %x\n", pickedMode);
    if (pickedMode != 0xFFFF) printf("  Width: %d\n  Height: %d\n  BPP: %d\n", modeInfo->width, modeInfo->height, modeInfo->bpp);
    /*if (pickedMode != 0xFFFF && VBE_SetMode(pickedMode)) {
      uint32_t* fb = (uint32_t*)modeInfo->framebuffer;
      int w = modeInfo->width;
      int h = modeInfo->height;
      int i = 0;
      while (true)
      {
        for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++)
          {
            fb[y * w + x] = COLOR(y, x, x * y);
          }
        }
        if (i++ > 99) i = 0;
      }
    }*/

    printf("VBE info:\n");
    printf("  Signature: %c%c%c%c\n", info->signature[0], info->signature[1], info->signature[2], info->signature[3]);
    printf("  Version: %x\n", info->version);
    printf("  OEM String: %x\n", info->oem_string_ptr);
    printf("  Capabilities: %x\n", info->capabilities);
    printf("  Video Mode Ptr: %x\n", info->video_mode_ptr);
    printf("  Total Memory: %x\n", info->total_memory);
  } else {
    printf("Can't get controller info\n");
  }

end:
  for (;;);
}
