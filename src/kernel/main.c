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
#include <arch/gdt.h>
#include <arch/idt.h>

extern uint8_t __bss_start;
extern uint8_t __end;

void __attribute__((cdecl)) start(uint16_t drive) {
  cls();

  disk_t disk;
  printf("Initializing DISK...");
  if (!disk_init(&disk, drive)) {
    printf("Disk init failed");
    printERR();
    goto end;
  } else {
    printOK();
  }

  printf("Initializing FAT...");
  if (!fat_init(&disk)) {
    printf("FAT init failed");
    printERR();
    goto end;
  } else {
    printOK();
  }

    // Initialize VESA
  printf("Initializing VESA...");
  if (video_init() != 0) {
    printf("VESA init failed");
    printERR();
    goto end;
  } else {
    printOK();
  }

  // Enable VESA
  printf("Enabling VESA...");
  printERR(); // Disable for now
  /*if (video_enable_VESA() != 0) {
    printf("VESA enable failed");
    printERR();
    goto end;
  } else {
    printOK();
  }*/

  memset(&__bss_start, 0, &__end - &__bss_start);

  // Initialize GDT
  printf("Initializing GDT...");
  GDT_init();
  printOK();

  // Initialize IDT
  printf("Initializing IDT...");
  IDT_init();
  printOK();

  // Read test.txt file
  char buffer[512];
  fat_file_t* file = fat_open(&disk, "/test.txt");
  if (!file) {
    printERR();
    goto end;
  }
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
