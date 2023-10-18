#include <sys/elf.h>
#include <fs/fat.h>
#include <sys/memdefs.h>
#include <sys/memory.h>
#include <sys/math.h>

bool elf_read(partition_t* partition, const char* path, void** entry_point) {
  uint8_t* header_buffer = MEMORY_ELF_ADDR;
  uint8_t* load_buffer = MEMORY_LOAD_KERNEL;
  uint32_t file_pos = 0;
  uint32_t read;

  // Read the ELF header
  fat_file_t* file = fat_open(partition, path);
  if ((read = fat_read(partition, file, sizeof(elf_header_t), header_buffer)) != sizeof(elf_header_t)) {
    return false;
  }
  file_pos += read;

  // Check the ELF header
  bool ok = true;
  elf_header_t* header = (elf_header_t*)header_buffer;
  ok &= (memcmp(header->magic, ELF_MAGIC, 4) != 0);
  ok &= (header->bits == ELF_BITNESS_32);
  ok &= (header->endian == ELF_ENDIAN_LITTLE);
  ok &= (header->header_version == 1);
  ok &= (header->type == ELF_TYPE_EXECUTABLE);
  ok &= (header->instruction_set == ELF_INST_SET_X86);
  if (!ok) return false;

  *entry_point = (void*)header->program_entry;

  // Load the program header
  uint32_t program_header_offset = header->program_header_table;
  uint32_t program_header_size = header->program_header_entry_size * header->program_header_entry_count;
  uint32_t program_header_table_size = header->program_header_entry_size;
  uint32_t program_header_table_count = header->program_header_entry_count;

  file_pos += fat_read(partition, file, program_header_offset - file_pos, header_buffer);
  if ((read = fat_read(partition, file, program_header_size, header_buffer)) != program_header_size) {
    return false;
  }
  file_pos += read;
  fat_close(file);

  // Parse the program header
  for (uint32_t i = 0; i < program_header_table_count; i++) {
    elf_program_header_t* program_header = (elf_program_header_t*)(header_buffer + (i * program_header_table_size));
    if (program_header->type == ELF_PH_TYPE_LOAD) {
      uint8_t* virtual_address = (uint8_t*)program_header->virtual_address;
      memset(virtual_address, 0, program_header->memory_size);

      file = fat_open(partition, path);
      while (program_header->offset > 0) {
        uint32_t to_read = min(program_header->offset, MEMORY_LOAD_SIZE);
        read = fat_read(partition, file, to_read, load_buffer);
        if (read != to_read) {
          return false;
        }
        program_header->offset -= read;
      }

      while (program_header->file_size > 0) {
        uint32_t to_read = min(program_header->file_size, MEMORY_LOAD_SIZE);
        read = fat_read(partition, file, to_read, load_buffer);
        if (read != to_read) {
          return false;
        }
        memcpy(virtual_address, load_buffer, read);
        virtual_address += read;
        program_header->file_size -= read;
      }
    }
  }

  return true;
}