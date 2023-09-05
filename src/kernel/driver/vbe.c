#include <stddef.h>
#include "vbe.h"
#include "x86.h"
#include "../lib/memory.h"

bool VBE_GetControllerInfo(vbe_info_t* info) {
  if (info == NULL) return false;

  if (x86_Video_GetVbeInfo(info) == 0) {
    info->video_mode_ptr = SEG_OFF_2_LIN(info->video_mode_ptr);
    return true;
  }

  return true;
}

bool VBE_GetModeInfo(uint16_t mode, vbe_mode_info_t* info) {
  if (info == NULL) return false;

  if (x86_Video_GetModeInfo(mode, info) == 0) {
    return true;
  }

  return false;
}

bool VBE_SetMode(uint16_t mode) {
  return x86_Video_SetMode(mode) == 0;
}