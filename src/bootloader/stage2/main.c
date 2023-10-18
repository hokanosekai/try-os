#include <fs/fat.h>
#include <sys/memdefs.h>
#include <sys/elf.h>
#include <boot/params.h>
#include <libc/stdio.h>

uint8_t* kernel_load_buffer = (uint8_t*)MEMORY_LOAD_KERNEL;
uint8_t* kernel = (uint8_t*)MEMORY_KERNEL_ADDR;

boot_params_t g_BootParams;

typedef void (*kernel_start_t)(boot_params_t* params);

void __attribute((cdecl)) start(uint16_t boot_drive, void* part) {
  cls();

  disk_t disk;
  if (!disk_init(&disk, boot_drive)) {
    printf("Failed to initialize disk!\n");
    goto end;
  }

  partition_t partition;
  mbr_detect_part(&partition, &disk, part);

  if (!fat_init(&partition)) {
    printf("Failed to initialize FAT!\n");
    goto end;
  }

  g_BootParams.boot_device = boot_drive;

  kernel_start_t kernel_entry;
  if (!elf_read(&partition, "/boot/kernel.elf", (void**)&kernel_entry)) {
    printf("Failed to read kernel!\n");
    goto end;
  }

  kernel_entry(&g_BootParams);

end:
  for (;;);
}