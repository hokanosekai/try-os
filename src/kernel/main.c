#include <stdint.h>
#include "lib/stdio.h"
#include "lib/memdefs.h"
#include "lib/memory.h"
#include "driver/disk.h"
#include "lib/window.h"
#include "driver/fat.h"
#include "lib/colors.h"
#include <stddef.h>

static window_t* g_Window = NULL;

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

  // init window
  if (!window_init(g_Window, 800, 600, 32)) {
    printf("Window init failed\n");
    goto end;
  }

  //window_clear(g_Window, COLOR_BLACK);
  /*window_draw_pixel(g_Window, 45, 56, COLOR_GREEN);

  window_draw_line(g_Window, 0, 0, 799, 599, COLOR_WHITE);

  window_draw_rect(g_Window, 100, 100, 200, 200, COLOR_BLUE);
  window_draw_rect_fill(g_Window, 300, 300, 200, 200, COLOR_RED);

  window_draw_circle(g_Window, 400, 300, 100, COLOR_YELLOW);
  window_draw_circle_fill(g_Window, 400, 300, 50, COLOR_GREEN);

  window_draw_triangle(g_Window, 100, 100, 200, 200, 300, 100, COLOR_CYAN);
  window_draw_triangle_fill(g_Window, 95, 30, 20, 40, 67, 30, COLOR_WHITE);*/

  window_draw_pixel(g_Window, 100, 100, COLOR_GREEN);
  window_draw_char(g_Window, 100, 100, 'A', COLOR_WHITE);
  window_draw_char(g_Window, 110, 100, 'B', COLOR_WHITE);
  window_draw_char(g_Window, 120, 100, 'C', COLOR_WHITE);
  window_draw_char(g_Window, 130, 100, 'D', COLOR_WHITE);
  window_draw_char(g_Window, 140, 100, 'E', COLOR_WHITE);
  window_draw_char(g_Window, 150, 100, 'F', COLOR_WHITE);

  window_draw_string(g_Window, 100, 120, "ABC DEF GHI JKL MNO PQR\n STU VWX YZ@ 0123456789! .,:;!? +-*/=\\_<>|{}()[]#%", COLOR_WHITE);

  window_draw_string(g_Window, 100, 160, buffer, COLOR(34, 65, 178));

end:
  for (;;);
}
