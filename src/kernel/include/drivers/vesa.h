#ifndef __DRIVER_VESA_H__
#define __DRIVER_VESA_H__

#include <libc/stdint.h>
#include <libc/stdbool.h>
#include <sys/memdefs.h>

typedef struct {
  char signature[4];                  // "VESA" 4 bytes
  uint16_t version;                   // VBE version number (0x0300 for VBE 3.0)
  uint32_t oem_string_ptr;            // Pointer to OEM string
  uint8_t capabilities[4];            // Capabilities of graphics controller
  uint32_t video_mode_ptr;            // Pointer to video mode list
  uint16_t total_memory;              // Number of 64kb memory blocks
  uint8_t reserved[236 + 256];        // Reserved
} __attribute__((packed)) vbe_info_t;

typedef struct {
  uint16_t attributes;                // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
  uint8_t window_a;                   // deprecated
  uint8_t window_b;                   // deprecated
  uint16_t granularity;               // deprecated; used while calculating bank numbers
  uint16_t window_size;
  uint16_t segment_a;
  uint16_t segment_b;
  uint32_t win_func_ptr;              // deprecated; used to switch banks from protected mode without returning to real mode
  uint16_t pitch;                     // number of bytes per horizontal line
  uint16_t width;                     // width in pixels
  uint16_t height;                    // height in pixels
  uint8_t w_char;                     // unused...
  uint8_t y_char;                     // ...
  uint8_t planes;
  uint8_t bpp;                        // bits per pixel in this mode
  uint8_t banks;                      // deprecated; total number of banks in this mode
  uint8_t memory_model;
  uint8_t bank_size;                  // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
  uint8_t image_pages;
  uint8_t reserved0;

  uint8_t red_mask;
  uint8_t red_position;
  uint8_t green_mask;
  uint8_t green_position;
  uint8_t blue_mask;
  uint8_t blue_position;
  uint8_t reserved_mask;
  uint8_t reserved_position;
  uint8_t direct_color_attributes;

  uint32_t framebuffer;               // physical address of the linear frame buffer; write here to draw to the screen
  uint32_t off_screen_mem_off;
  uint16_t off_screen_mem_size;       // size of memory in the framebuffer but not being displayed on the screen
  uint8_t reserved1[206];
} __attribute__((packed)) vbe_mode_info_t;

bool VESA_GetControllerInfo(vbe_info_t *info);
bool VESA_GetModeInfo(uint16_t mode, vbe_mode_info_t *info);
bool VESA_SetMode(uint16_t mode);
bool VESA_isInitialized();
void VESA_SetInitialized(bool value);
void VESA_SetPixel(uint32_t x, uint32_t y, uint32_t color);
void VESA_clear(uint32_t color);

#endif