#pragma once

#include <stdint.h>
#include "disk.h"

typedef struct {
  uint8_t name[11];
  uint8_t attributes;
  uint8_t _reserved;
  uint8_t creation_time_ms;
  uint16_t creation_time;
  uint16_t creation_date;
  uint16_t last_access_date;
  uint16_t first_cluster_high;
  uint16_t last_write_time;
  uint16_t last_write_date;
  uint16_t first_cluster_low;
  uint32_t file_size;
} __attribute__((packed)) fat_direntry_t;

typedef struct {
  int handle;
  bool is_directory;
  uint32_t position;
  uint32_t size;
} fat_file_t;

enum FAT_Attributes {
  FAT_ATTR_READ_ONLY      = 0x01,
  FAT_ATTR_HIDDEN         = 0x02,
  FAT_ATTR_SYSTEM         = 0x04,
  FAT_ATTR_VOLUME_ID      = 0x08,
  FAT_ATTR_DIRECTORY      = 0x10,
  FAT_ATTR_ARCHIVE        = 0x20,
  FAT_ATTR_LFN            = FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID,
};

bool fat_init(disk_t *disk);
fat_file_t *fat_open(disk_t *disk, const char *path);
uint32_t fat_read(disk_t *disk, fat_file_t *file, uint32_t byteCount, void* dataOut);
bool fat_read_entry(disk_t *disk, fat_file_t *file, fat_direntry_t *entry);
void fat_close(fat_file_t *file);