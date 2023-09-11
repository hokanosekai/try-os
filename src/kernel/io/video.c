#include <io/video.h>
#include <io/io.h>
#include <drivers/vesa.h>
#include <lib/colors.h>
#include <libc/stdio.h>

extern vbe_mode_info_t* video_mode_info;
extern vbe_info_t* video_info;
extern uint16_t VESA_MODE;

// Default variables
uint8_t *VIDEO_MEM = (uint8_t *)MEMORY_VIDEO_ADDR;
uint8_t *VIDEO_PTR = (uint8_t *)MEMORY_VIDEO_ADDR;
int VIDEO_COLOR = 0x0F;

uint8_t up_buffer[SCREEN_HEIGHT][SCREEN_WIDTH * 2];
uint8_t down_buffer[SCREEN_HEIGHT][SCREEN_WIDTH * 2];

int video_x = 0, video_y = 0;
int is_scrolled = 0;
int is_shifted = 0;
int last_tab = 0;

int video_init() {
  VESA_MODE = 0xFFFF;

  if (VESA_GetControllerInfo(video_info)) {
    uint16_t* mode = (uint16_t*)video_info->video_mode_ptr;
    for (int i = 0; mode[i] != 0xFFFF; i++) {
      if (!VESA_GetModeInfo(mode[i], video_mode_info)) {
        continue;
      }
      bool hasFB = (video_mode_info->attributes & 0x90) == 0x90;
      if (hasFB && video_mode_info->width == 1024 && video_mode_info->height == 768 && video_mode_info->bpp == 32) {
        VESA_MODE = mode[i];
        break;
      }
    }
  }

  // No suitable mode found
  if (VESA_MODE == 0xFFFF) return -1;

  return 0;
}

int video_disable_VESA() {
  if (!VESA_isEnabled()) return -1;
  if (!VESA_SetMode(VESA_MODE_DISABLE)) return -1;
  VESA_SetEnabled(false);
  return 0;
}

int video_enable_VESA() {
  if (VESA_isEnabled()) return -1;
  if (!VESA_SetMode(VESA_MODE)) return -1;
  VESA_SetEnabled(true);
  return 0;
}

void video_cls() {
  if (VESA_isEnabled()) {
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
  if (VESA_isEnabled()) {
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
        break;
    }
    video_shiftall();
    video_setautocursor();
  }
}

void video_puts(const char* s) {
  while (*s != 0) {
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
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (SCREEN_WIDTH * 2)) + 1) * (SCREEN_WIDTH * 2));
    video_shiftall();
    video_setautocursor();
  }
}

void video_minline() {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (SCREEN_WIDTH * 2)) - 1) * (SCREEN_WIDTH * 2));
    video_newline();
    video_shiftall();
    video_setautocursor();
  }
}

void video_backspace() {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    if (last_tab) {
      int i;
      for (i = 0; i < 7; i++) {
        VIDEO_PTR -= 2;
        *VIDEO_PTR = 0x20;
      }
      last_tab--;
    }
  }
}

void video_tab() {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    VIDEO_PTR += 7 * 2;
  }
}

void video_setcolor(const int color) {
  VIDEO_COLOR = color;
}

void video_shiftall() {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    char *i;
    int index;

    if (VIDEO_PTR >= VIDEO_MEM + ((SCREEN_HEIGHT) * SCREEN_WIDTH * 2)) {
      video_rotate_buffer();

      for (index = 0; index < SCREEN_WIDTH * 2; index++) {
        up_buffer[SCREEN_HEIGHT - 1][index] = *(VIDEO_MEM + index);
      }

      for (i = VIDEO_MEM; i <= (VIDEO_MEM + (SCREEN_HEIGHT) * SCREEN_WIDTH * 2); i++) {
        *i = i[SCREEN_WIDTH * 2];
      }

      VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (SCREEN_WIDTH * 2)) - 1) * (SCREEN_WIDTH * 2));
      is_shifted = 1;
    }
  }
}

void video_rotate_buffer() {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    int y = 1;
    int x = 0;

    for (y = 1; y < SCREEN_HEIGHT; y++) {
      for (x = 0; x < SCREEN_WIDTH * 2; x++) {
        up_buffer[y - 1][x] = up_buffer[y][x];
      }
    }
  }
}

void video_scrolldown() {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    int y, x = 0;
    char *ptr = VIDEO_MEM;

    if (is_scrolled == 0 || is_shifted == 0) return;

    for (; y < SCREEN_HEIGHT; y++) {
      for (; x < SCREEN_WIDTH * 2; x++) {
        *ptr++ = down_buffer[y][x];
      }
    }
    is_scrolled = 0;
    video_gotoxy(video_x, video_y);
    video_x = 0;
    video_y = 0;
  }
}

void video_scrollup() {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    int y, x = 0;
    char *ptr = VIDEO_MEM;

    if (is_scrolled == 1 || is_shifted == 0) return;

    for (; y < SCREEN_HEIGHT; y++) {
      for (; x < SCREEN_WIDTH * 2; x++) {
        down_buffer[y][x] = *ptr;
        *ptr++ = up_buffer[y][x];
      }
    }
    is_scrolled = 1;
    video_x = video_getcolumn();
    video_y = video_getrow();
    video_gotoxy(SCREEN_WIDTH, SCREEN_HEIGHT);
  }
}

int video_getcolumn() {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    int column = 0;
    column = (VIDEO_PTR - VIDEO_MEM) % (SCREEN_WIDTH * 2);
    return column / 2;
  }
}

int video_getrow() {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    int row;
    row = (VIDEO_PTR - VIDEO_MEM) / (SCREEN_WIDTH * 2);
    return row;
  }
}

void video_gotoxy(int x, int y) {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    VIDEO_PTR = VIDEO_MEM + ((y * SCREEN_WIDTH * 2) + (x * 2));
    video_setautocursor();
  }
}

void video_setcursor(int x, int y) {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    int pos = (y * SCREEN_WIDTH) + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
  }
}

void video_setautocursor() {
  if (VESA_isEnabled()) {
    // TODO: Implement this (in graphics.c)
  } else {
    video_setcursor(
      ((VIDEO_PTR - VIDEO_MEM) / 2) / SCREEN_WIDTH,
      ((VIDEO_PTR - VIDEO_MEM) / 2) % SCREEN_WIDTH
    );
  }
}