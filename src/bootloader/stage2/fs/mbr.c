#include <fs/mbr.h>
#include <sys/memory.h>

void MBR_detect_part(partition_t* part, disk_t* disk, void* partition) {
  part->disk = disk;
  if (disk->id < 0x80) {
    part->partition_offset = 0;
    part->partition_size = disk->cylinder * disk->sectors * disk->heads;
  } else {
    mbr_entry_t* entry = (mbr_entry_t*)seg2lin(partition);
    part->partition_offset = entry->lba_start;
    part->partition_size = entry->size;
  }
}

bool MBR_read_part(partition_t* part, uint32_t lba, uint8_t sectors, void* dataOut) {
  return disk_read(part->disk, part->partition_offset + lba, sectors, dataOut);
}