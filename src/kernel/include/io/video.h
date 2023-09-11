#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <drivers/vesa.h>
#include <sys/memdefs.h>

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

int video_init(void);
int video_disable_VESA(void);
int video_enable_VESA(void);

void video_cls(void);
void video_setcolor(const int color);
void video_putc(char c);
void video_puts(const char *s);
void video_scrollup(void);
void video_scrolldown(void);

void video_newline(void);
void video_minline(void);
void video_backspace(void);
void video_tab(void);

void video_shiftall(void);
void video_gotoxy(int x, int y);
void video_setcursor(int x, int y);
void video_setautocursor();
void video_rotate_buffer(void);

int video_getcolumn();
int video_getrow();

#endif

