#include "disk.h"
#include "x86.h"
#include "../lib/stdio.h"

bool disk_init(disk_t* disk, uint8_t id) {
  uint8_t type;
  uint16_t cylinders, sectors, heads;

  if (!x86_Disk_GetDriveParams(disk->id, &type, &cylinders, &sectors, &heads)) {
    return false;
  }

  disk->id = id;
  disk->cylinder = cylinders;
  disk->sectors = sectors;
  disk->heads = heads;

  return true;
}

void disk_LBA2CHS(disk_t* disk, 
                  uint32_t lba, 
                  uint16_t* cylinder, 
                  uint16_t* sector, 
                  uint16_t* head
                  ) {
  // sector = (lba % sectors) + 1
  *sector = (lba % disk->sectors) + 1;

  // cylinder = (lba / sectors) / heads
  *cylinder = (lba / disk->sectors) / disk->heads;

  // head = (lba / sectors) % heads
  *head = (lba / disk->sectors) % disk->heads;
}

bool disk_read(disk_t* disk, uint32_t lba, uint8_t sectors, void* dataOut) {
  uint16_t cylinder, sector, head;
  disk_LBA2CHS(disk, lba, &cylinder, &sector, &head);

  for (int i = 0; i < 3; i++) {
    if (x86_Disk_Read(disk->id, cylinder, sector, head, sectors, dataOut)) {
      return true;
    }

    x86_Disk_Reset(disk->id);
  }

  return false;
}
