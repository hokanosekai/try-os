#ifndef __FAT_H__
#define __FAT_H__

#include <libc/stdint.h>
#include <fs/mbr.h>

#define FAT_LFN_LAST    0x40

typedef struct {
  uint8_t name[11];
  uint8_t attr;
  uint8_t _reserved;
  uint8_t ctime_ms;
  uint16_t ctime;
  uint16_t cdate;
  uint16_t adate;
  uint16_t cluster_hi;
  uint16_t mtime;
  uint16_t mdate;
  uint16_t cluster_lo;
  uint32_t size;
} __attribute__((packed)) fat_dirent_t;

typedef struct {
  uint8_t order;
  uint16_t name1[5];
  uint8_t attr;
  uint8_t type;
  uint8_t checksum;
  uint16_t name2[6];
  uint16_t _reserved;  // Always 0
  uint16_t name3[2];
} __attribute__((packed)) fat_long_filent_t;

typedef struct {
  int handle;
  bool is_dir;
  uint32_t pos;
  uint32_t size;
} __attribute__((packed)) fat_file_t;

typedef enum {
  FAT_ATTR_READ_ONLY  = 0x01,
  FAT_ATTR_HIDDEN     = 0x02,
  FAT_ATTR_SYSTEM     = 0x04,
  FAT_ATTR_VOLUME_ID  = 0x08,
  FAT_ATTR_DIRECTORY  = 0x10,
  FAT_ATTR_ARCHIVE    = 0x20,
  FAT_ATTR_LFN        = FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID,
} FAT_ATTR;

bool FAT_init(partition_t *part);
fat_file_t *FAT_open(partition_t *part, const char *path);
uint32_t FAT_read(partition_t *part, fat_file_t *file, uint32_t byteCount, void* dataOut);
bool FAT_read_entry(partition_t *part, fat_file_t *file, fat_dirent_t *entry);
void FAT_close(fat_file_t *file);

#endif