#ifndef __MBR_H__
#define __MBR_H__

#include <drivers/disk.h>
#include <libc/stdint.h>
#include <libc/stdbool.h>

typedef struct {
  uint8_t attributes;
  uint8_t chs_start[3];
  uint8_t type;
  uint8_t chs_end[3];
  uint32_t lba_start;
  uint32_t size;
} __attribute__((packed)) mbr_entry_t;

typedef struct {
  disk_t* disk;
  uint32_t partition_offset;
  uint32_t partition_size;
} partition_t;

void mbr_detect_part(partition_t* part, disk_t* disk, void* partition);
bool mbr_read_part(partition_t* part, uint32_t lba, uint8_t sectors, void* dataOut);

#endif