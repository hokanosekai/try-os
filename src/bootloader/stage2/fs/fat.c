#include <fs/fat.h>
#include <libc/string.h>
#include <libc/stdio.h>
#include <libc/ctype.h>
#include <libc/stddef.h>
#include <sys/math.h>
#include <sys/memory.h>
#include <sys/memdefs.h>
#include <drivers/disk.h>

#define SECTOR_SIZE         512
#define MAX_PATH_SIZE       256
#define MAX_FILE_HANDLES    10
#define ROOT_DIR_HANDLE     -1
#define FAT_CACHE_SIZE      5

typedef struct {
  uint8_t drive_nb;
  uint8_t _reserved;
  uint8_t signature;
  uint32_t volume_id;
  uint8_t volume_label[11];
  uint8_t system_id;
} __attribute__((packed)) fat_extended_boot_record_t;

typedef struct {
  uint32_t sectors_per_fat;
  uint16_t flags;
  uint16_t version;
  uint32_t root_cluster;
  uint16_t fsinfo_sector;
  uint16_t backup_boot_sector;
  uint8_t _reserved[12];
  fat_extended_boot_record_t EBR;
} __attribute__((packed)) fat32_extended_boot_record_t;

typedef struct {
  uint8_t boot_jmp_inst[3];           // Boot jump instruction
  uint8_t oem_name[8];                // OEM name and version
  uint16_t bytes_per_sector;          // Bytes per sector
  uint8_t sectors_per_cluster;        // Sectors per cluster
  uint16_t reserved_sectors;          // Reserved sectors
  uint8_t num_fats;                   // Number of FATs
  uint16_t root_entry_count;          // Root entry count
  uint16_t total_sectors;             // Total sectors
  uint8_t media_type;                 // Media type
  uint16_t sectors_per_fat;           // Sectors per FAT
  uint16_t sectors_per_track;         // Sectors per track
  uint16_t num_heads;                 // Number of heads
  uint32_t hidden_sectors;            // Hidden sectors
  uint32_t large_sector_count;        // Large sector count

  union {
    fat_extended_boot_record_t EBR1216;
    fat32_extended_boot_record_t EBR32;
  };
} __attribute__((packed)) fat_boot_sector_t;

typedef struct {
  uint8_t buffer[SECTOR_SIZE];
  fat_file_t public;
  bool opened;
  uint32_t first_cluster;
  uint32_t current_cluster;
  uint32_t current_sector;
} fat_file_data_t;

typedef struct {
  uint8_t order;
  int16_t chars[3];
} fat_LFN_block_t;

typedef struct {
  union {
    fat_boot_sector_t boot_sector;
    uint8_t boot_sector_bytes[SECTOR_SIZE];
  } BS;

  fat_file_data_t root_dir;
  fat_file_data_t file_handles[MAX_FILE_HANDLES];

  uint8_t fat_cache[FAT_CACHE_SIZE * SECTOR_SIZE];
  uint32_t fat_cache_pos;

  fat_LFN_block_t LFN_blocks[FAT_LFN_LAST];
  int LFN_block_count;
} fat_data_t;

static fat_data_t* g_Data;
static uint32_t g_DataSectionLba;
static uint8_t g_FatType;
static uint32_t g_TotalSectors;
static uint32_t g_SectorsPerFat;

uint32_t fat_cluster_to_lba(uint32_t cluster);

int fat_compare_LFN_blocks(const void* block_a, const void* block_b) {
  fat_LFN_block_t* a = (fat_LFN_block_t*)block_a;
  fat_LFN_block_t* b = (fat_LFN_block_t*)block_b;

  return ((int)a->order) - ((int)b->order);
}

bool fat_read_boot_sector(partition_t* part) {
  return mbr_read_part(part, 0, 1, g_Data->BS.boot_sector_bytes);
}

bool fat_read_fat(partition_t* part, size_t lba_index) {
  return mbr_read_part(part, g_Data->BS.boot_sector.reserved_sectors + lba_index, FAT_CACHE_SIZE, g_Data->fat_cache);
}

void fat_detect(partition_t *part) {
  uint32_t dataCluster = (g_TotalSectors - g_DataSectionLba) / g_Data->BS.boot_sector.sectors_per_cluster;
  if (dataCluster < 0xFF5) {
    g_FatType = 12;
  } else if (g_Data->BS.boot_sector.sectors_per_fat != 0) {
    g_FatType = 16;
  } else {
    g_FatType = 32;
  }
}

bool fat_init(partition_t* part) {
  g_Data = (fat_data_t*)MEMORY_FAT_ADDR;

  // Read boot sector
  if (!fat_read_boot_sector(part)) {
    printf("FAT: Failed to read boot sector\r\n");
    return false;
  }

  // Read FAT
  g_Data->fat_cache_pos = 0xFFFFFFFF;

  g_TotalSectors = g_Data->BS.boot_sector.total_sectors;
  if (g_TotalSectors == 0)
    g_TotalSectors = g_Data->BS.boot_sector.large_sector_count;

  bool isFat32 = false;
  g_SectorsPerFat = g_Data->BS.boot_sector.sectors_per_fat;
  if (g_SectorsPerFat == 0) {
    isFat32 = true;
    g_SectorsPerFat = g_Data->BS.boot_sector.EBR32.sectors_per_fat;
  }

  // Open root directory
  uint32_t root_dir_lba;
  uint32_t root_dir_size;
  if (isFat32) {
    g_DataSectionLba = g_Data->BS.boot_sector.reserved_sectors + g_SectorsPerFat * g_Data->BS.boot_sector.num_fats;
    root_dir_lba = fat_cluster_to_lba(g_Data->BS.boot_sector.EBR32.root_cluster);
    root_dir_size = 0;
  } else {
    root_dir_lba = g_Data->BS.boot_sector.reserved_sectors + g_SectorsPerFat * g_Data->BS.boot_sector.num_fats;
    root_dir_size = g_Data->BS.boot_sector.root_entry_count * sizeof(fat_dirent_t);
    uint32_t root_dir_sectors = (root_dir_size + g_Data->BS.boot_sector.bytes_per_sector - 1) / g_Data->BS.boot_sector.bytes_per_sector;
    g_DataSectionLba = root_dir_lba + root_dir_sectors;
  }

  g_Data->root_dir.public.handle          = ROOT_DIR_HANDLE;
  g_Data->root_dir.public.is_dir          = true;
  g_Data->root_dir.public.pos             = 0;
  g_Data->root_dir.public.size            = sizeof(fat_dirent_t) * g_Data->BS.boot_sector.root_entry_count;
  g_Data->root_dir.opened                 = true;
  g_Data->root_dir.first_cluster          = root_dir_lba;
  g_Data->root_dir.current_cluster        = root_dir_lba;
  g_Data->root_dir.current_sector         = 0;

  if (!mbr_read_part(part, root_dir_lba, 1, g_Data->root_dir.buffer)) {
    printf("FAT: Failed to read root directory\r\n");
    return false;
  }

  // Calculate data section LBA
  fat_detect(part);

  // Reset file handles
  for (int i = 0; i < MAX_FILE_HANDLES; i++) {
    g_Data->file_handles[i].opened = false;
  }
  g_Data->LFN_block_count = 0;

  return true;
}

uint32_t fat_cluster_to_lba(uint32_t cluster) {
  return g_DataSectionLba + (cluster - 2) * g_Data->BS.boot_sector.sectors_per_cluster;
}

fat_file_t* fat_open_entry(partition_t* part, fat_dirent_t* entry) {
  // Find free file handle
  int handle = -1;
  for (int i = 0; i < MAX_FILE_HANDLES; i++) {
    if (!g_Data->file_handles[i].opened) {
      handle = i;
    }
  }

  if (handle < 0) {
    printf("FAT: No free file handles\r\n");
    return NULL;
  }

  // Setup file handle
  fat_file_data_t* file       = &g_Data->file_handles[handle];
  file->public.handle         = handle;
  file->public.is_dir         = (entry->attr & FAT_ATTR_DIRECTORY) != 0;
  file->public.pos            = 0;
  file->public.size           = entry->size;
  file->first_cluster         = entry->cluster_lo + ((uint32_t)entry->cluster_hi << 16);
  file->current_cluster       = file->first_cluster;
  file->current_sector        = 0;

  // Read first cluster
  if (!mbr_read_part(part, fat_cluster_to_lba(file->current_cluster), 1, file->buffer)) {
    printf("FAT: Failed to read file\r\n");
    printf("\tCluster: %u\r\n", file->current_cluster);
    printf("\tLBA: %u\r\n", fat_cluster_to_lba(file->current_cluster));

    for (size_t i = 0; i < 11; i++)
      printf("\t%c ", entry->name[i]);
    printf("\r\n");

    return false;
  }

  file->opened = true;
  return &file->public;
}

uint32_t fat_next_cluster(partition_t* part, uint32_t current_cluster) {
  // Determine the byte offset of the entry we need to read
  uint32_t fat_index;
  if (g_FatType == 12) {
    fat_index = current_cluster * 3 / 2;
  } else if (g_FatType == 16) {
    fat_index = current_cluster * 2;
  } else /*if (g_FatType == 32)*/ {
    fat_index = current_cluster * 4;
  }

  // Make sure cache has the right number
  uint32_t fat_index_sector = fat_index / SECTOR_SIZE;
  if (fat_index_sector < g_Data->fat_cache_pos || fat_index_sector >= g_Data->fat_cache_pos + FAT_CACHE_SIZE) {
    if (!fat_read_fat(part, fat_index_sector)) {
      printf("FAT: Failed to read FAT\r\n");
      return 0;
    }

    g_Data->fat_cache_pos = fat_index_sector;
  }

  fat_index -= (g_Data->fat_cache_pos * SECTOR_SIZE);

  uint32_t next_cluster;
  if (g_FatType == 12) {
    if (current_cluster % 2 == 0)
      next_cluster = (*(uint16_t*)(g_Data->fat_cache + fat_index)) & 0x0FFF;
    else
      next_cluster = (*(uint16_t*)(g_Data->fat_cache + fat_index)) >> 4;

    if (next_cluster >= 0xFF8)
      next_cluster |= 0xFFFF0000;
  } else if (g_FatType == 16) {
    next_cluster = *(uint16_t*)(g_Data->fat_cache + fat_index);
    if (next_cluster >= 0xFFF8)
      next_cluster |= 0xFFFF0000;
  } else /*if (g_FatType == 32)*/ {
    next_cluster = *(uint32_t*)(g_Data->fat_cache + fat_index);
  }

  return next_cluster;
}

uint32_t fat_read(partition_t* part, fat_file_t* file, uint32_t byte_count, void* data_out) {
  // Get file handle
  fat_file_data_t* fd = (file->handle == ROOT_DIR_HANDLE)
    ? &g_Data->root_dir
    : &g_Data->file_handles[file->handle];

  uint8_t* u8_data_out = (uint8_t*)data_out;

  // Don't read past end of file
  if (!fd->public.is_dir || (fd->public.is_dir && fd->public.size != 0))
    byte_count = min(byte_count, fd->public.size - fd->public.pos);

  // Read data
  while (byte_count > 0) {
    uint32_t left_in_buffer = SECTOR_SIZE - (fd->public.pos % SECTOR_SIZE);
    uint32_t take = min(byte_count, left_in_buffer);

    memcpy(u8_data_out, fd->buffer + fd->public.pos % SECTOR_SIZE, take);
    u8_data_out += take;
    fd->public.pos += take;
    byte_count -= take;

    if (left_in_buffer == take) {
      if (fd->public.handle == ROOT_DIR_HANDLE) {
        ++fd->current_cluster;

        // Read next sector
        if (!mbr_read_part(part, fd->current_cluster, 1, fd->buffer)) {
          printf("FAT: Failed to read root directory\r\n");
          break;
        }
      } else {
        // Calculate next cluster
        if (++fd->current_sector >= g_Data->BS.boot_sector.sectors_per_cluster) {
          fd->current_sector = 0;
          fd->current_cluster = fat_next_cluster(part, fd->current_cluster);
        }

        if (fd->current_cluster >= 0xFFFFFFF8) {
          // Mark as end of file
          fd->public.size = fd->public.pos;
          break;
        }

        // Read next sector
        if (!mbr_read_part(part, fat_cluster_to_lba(fd->current_cluster) + fd->current_sector, 1, fd->buffer)) {
          printf("FAT: Failed to read file\r\n");
          printf("\tCluster: %u\r\n", fd->current_cluster);
          printf("\tLBA: %u\r\n", fat_cluster_to_lba(fd->current_cluster));
          break;
        }
      }
    }
  }

  return u8_data_out - (uint8_t*)data_out;
}

bool fat_read_entry(partition_t* part, fat_file_t* file, fat_dirent_t* dir_entry) {
  return fat_read(part, file, sizeof(fat_dirent_t), dir_entry) == sizeof(fat_dirent_t);
}

void fat_close(fat_file_t* file) {
  if (file->handle == ROOT_DIR_HANDLE) {
    file->pos = 0;
    g_Data->root_dir.current_cluster = g_Data->root_dir.first_cluster;
  } else {
    g_Data->file_handles[file->handle].opened = false;
  }
}

void fat_get_short_name(const char* name, char short_name[12]) {
  memset(short_name, ' ', 12);
  short_name[11] = '\0';

  const char* ext = strchr(name, '.');
  if (ext == NULL)
    ext = name + 11;

  for (int i = 0; i < 8 && name[i] && name + i < ext; i++) {
    short_name[i] = toupper(name[i]);
  }

  if (ext != name + 11) {
    for (int i = 0; i < 3 && ext[i + 1]; i++) {
      short_name[i + 8] = toupper(ext[i + 1]);
    }
  }
}

bool fat_find_file(partition_t* part, fat_file_t* file, const char* name, fat_dirent_t* entry_out) {
  char short_name[12];
  fat_dirent_t entry;

  fat_get_short_name(name, short_name);

  while (fat_read_entry(part, file, &entry)) {
    if (memcmp(short_name, entry.name, 11) == 0) {
      *entry_out = entry;
      return true;
    }
  }

  return false;
}

fat_file_t* fat_open(partition_t* part, const char* path) {
  char name[MAX_PATH_SIZE];

  // ignore leading slashes
  if (path[0] == '/')
    path++;

  fat_file_t* current = &g_Data->root_dir.public;

  while (*path) {
    // extract next file name
    bool isLast = false;
    const char* delim = strchr(path, '/');
    if (delim != NULL) {
      memcpy(name, path, delim - path);
      name[delim - path + 1] = '\0';
      path = delim + 1;
    } else {
      unsigned len = strlen(path);
      memcpy(name, path, len);
      name[len + 1] = '\0';
      path += len;
      isLast = true;
    }

    // find file in current directory
    fat_dirent_t entry;
    if (fat_find_file(part, current, name, &entry)) {
      fat_close(current);

      // check if it's a directory
      if (!isLast && entry.attr & FAT_ATTR_DIRECTORY == 0) {
        printf("FAT: %s is not a directory\r\n", name);
        return NULL;
      }

      // open new directory entry
      current = fat_open_entry(part, &entry);
    } else {
      fat_close(current);

      printf("FAT: %s not found\r\n", name);
      return NULL;
    }
  }

  return current;
}