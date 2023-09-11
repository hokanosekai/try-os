#include <io/video.h>
#include <drivers/vesa.h>
#include <lib/colors.h>

extern vbe_mode_info_t* video_mode_info;
extern vbe_info_t* video_info;

// Default variables
uint8_t *VIDEO_MEM = (uint8_t *)MEMORY_VIDEO_ADDR;
uint8_t *VIDEO_PTR = (uint8_t *)MEMORY_VIDEO_ADDR;
int VIDEO_COLOR = 0x07;

uint8_t up_buffer[SCREEN_WIDTH * SCREEN_HEIGHT * 2];
uint8_t down_buffer[SCREEN_WIDTH * SCREEN_HEIGHT * 2];

int video_x = 0, video_y = 0;

int video_init() {
  uint16_t pickedMode = 0xFFFF;

  if (VESA_GetControllerInfo(video_info)) {
    uint16_t* mode = (uint16_t*)video_info->video_mode_ptr;
    for (int i = 0; mode[i] != 0xFFFF; i++) {
      if (!VESA_GetModeInfo(mode[i], video_mode_info)) {
        continue;
      }
      bool hasFB = (video_mode_info->attributes & 0x90) == 0x90;
      if (hasFB && video_mode_info->width == 1024 && video_mode_info->height == 768 && video_mode_info->bpp == 32) {
        pickedMode = mode[i];
        break;
      }
    }
  }

  // No suitable mode found
  if (pickedMode == 0xFFFF) return -1;

  // Set the mode
  if (!VESA_SetMode(pickedMode)) return -1;

  // Set the global variables
  VESA_SetInitialized(true);

  return 0;
}

int video_disable_VESA() {
  if (!VESA_isInitialized()) return -1;
  if (!VESA_SetMode(0x03)) return -1;
  VESA_SetInitialized(false);
  return 0;
}

void video_cls() {
  if (VESA_isInitialized()) {
    VESA_clear(COLOR_BLACK);
  } else {
    uint8_t* fb = (uint8_t*)VIDEO_MEM;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
      fb[i * 2] = ' ';
      fb[i * 2 + 1] = VIDEO_COLOR;
    }
    VIDEO_PTR = VIDEO_MEM;
  }
}

void video_putc(const char c) {
  if (VESA_isInitialized()) {
    // TODO: Implement this (in graphics.c)
  } else {
    if (video_x && video_y) video_scrolldown();

    switch (c) {
      case '\n':
        video_newline();
        break;
      case '\r':
        video_minline();
        break;
      case '\b':
        video_backspace();
        break;
      case '\033':
        break;
      case '\t':
        video_tab();
        break;
      default:
        *(VIDEO_PTR++) = c;
        *(VIDEO_PTR++) = VIDEO_COLOR;
        video_x++;
        break;
    }
  }
}

void video_puts(const char* s) {
  while (*s) {
    if (*s == '\n') {
      video_newline();
    } else if (*s == '\r') {
      video_minline();
    } else if (*s == '\b') {
      video_backspace();
    } else if (*s == '\033') {
      video_setcolor(*++s);
    } else if (*s == '\t') {
      video_tab();
    } else {
      video_putc(*s);
    }
    s++;
  }
}

void video_newline() {
  if (VESA_isInitialized()) {
    // TODO: Implement this (in graphics.c)
  } else {
    VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (SCREEN_WIDTH * 2)) + 1) * (SCREEN_WIDTH * 2));
    video_x = 0;
    video_y++;
  }
}

void video_minline() {
  if (VESA_isInitialized()) {
    // TODO: Implement this (in graphics.c)
  } else {
    VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (SCREEN_WIDTH * 2)) - 1) * (SCREEN_WIDTH * 2));
    video_newline();
  }
}

void video_backspace() {
  if (VESA_isInitialized()) {
    // TODO: Implement this (in graphics.c)
  } else {
    if (video_x) {
      VIDEO_PTR -= 2;
      *(VIDEO_PTR++) = ' ';
      *(VIDEO_PTR++) = VIDEO_COLOR;
      video_x--;
    }
  }
}

void video_tab() {
  if (VESA_isInitialized()) {
    // TODO: Implement this (in graphics.c)
  } else {
    VIDEO_PTR += 8 - (video_x % 8);
    video_x += 8 - (video_x % 8);
  }
}

void video_setcolor(const int color) {
  VIDEO_COLOR = color;
}

void video_scrolldown() {
  if (VESA_isInitialized()) {
    // TODO: Implement this (in graphics.c)
  } else {
    uint8_t* fb = (uint8_t*)VIDEO_MEM;
    for (int i = 0; i < SCREEN_WIDTH * (SCREEN_HEIGHT - 1); i++) {
      fb[i * 2] = fb[(i + SCREEN_WIDTH) * 2];
      fb[i * 2 + 1] = fb[(i + SCREEN_WIDTH) * 2 + 1];
    }
    for (int i = 0; i < SCREEN_WIDTH; i++) {
      fb[(SCREEN_WIDTH * (SCREEN_HEIGHT - 1) + i) * 2] = ' ';
      fb[(SCREEN_WIDTH * (SCREEN_HEIGHT - 1) + i) * 2 + 1] = VIDEO_COLOR;
    }
    VIDEO_PTR = VIDEO_MEM + (SCREEN_WIDTH * (SCREEN_HEIGHT - 1) * 2);
    video_x = 0;
    video_y--;
  }
}