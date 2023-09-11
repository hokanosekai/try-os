#include <libc/stdint.h>
#include <libc/stdio.h>
#include <sys/memdefs.h>
#include <sys/memory.h>
#include <drivers/disk.h>
#include <fs/fat.h>
#include <drivers/keyboard.h>
#include <lib/colors.h>
#include <libc/stddef.h>
#include <io/video.h>


void __attribute__((cdecl)) start(uint16_t drive) {
  // Initialize VESA
  printf("Initializing VESA...\n");
  if (video_init() != 0) {
    printf("VESA init failed\n");
    goto end;
  } else {
    video_disable_VESA();
    printf("VESA init success\n");
  }

  disk_t disk;
  printf("Initializing disk...\n");
  if (!disk_init(&disk, drive)) {
    printf("Disk init failed\n");
    goto end;
  } else {
    printf("Disk init success\n");
  }

  printf("Initializing FAT...\n");
  if (!fat_init(&disk)) {
    printf("FAT init failed\n");
    goto end;
  } else {
    printf("FAT init success\n");
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

end:
  for (;;);
}
