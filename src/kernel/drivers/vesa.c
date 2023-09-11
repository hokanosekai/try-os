#include <libc/stddef.h>
#include <drivers/vesa.h>
#include <drivers/x86.h>
#include <sys/memory.h>

bool is_vesa_initialized = false;

vbe_mode_info_t *video_mode_info = (vbe_mode_info_t *)MEMORY_MODE_INFO;
vbe_info_t *video_info = (vbe_info_t *)MEMORY_VESA_INFO;

bool VESA_GetControllerInfo(vbe_info_t* info) {
  if (info == NULL) return false;

  if (x86_Video_GetVbeInfo(info) == 0) {
    info->video_mode_ptr = SEG_OFF_2_LIN(info->video_mode_ptr);
    return true;
  }

  return true;
}

bool VESA_GetModeInfo(uint16_t mode, vbe_mode_info_t* info) {
  if (info == NULL) return false;

  if (x86_Video_GetModeInfo(mode, info) == 0) {
    return true;
  }

  return false;
}

bool VESA_SetMode(uint16_t mode) {
  return x86_Video_SetMode(mode) == 0;
}

bool VESA_isInitialized() {
  return is_vesa_initialized;
}

void VESA_SetInitialized(bool initialized) {
  is_vesa_initialized = initialized;
}

void VESA_SetPixel(uint32_t x, uint32_t y, uint32_t color) {
  if (!VESA_isInitialized()) return;

  uint32_t* fb = (uint32_t*)video_mode_info->framebuffer;
  fb[y * video_mode_info->width + x] = color;
}

void VESA_clear(uint32_t color) {
  if (!VESA_isInitialized()) return;

  uint32_t* fb = (uint32_t*)video_mode_info->framebuffer;
  for (int i = 0; i < video_mode_info->width * video_mode_info->height; i++) {
    fb[i] = color;
  }
}