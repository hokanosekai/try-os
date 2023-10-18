#ifndef __ELF_H__
#define __ELF_H__

#include <libc/stdint.h>
#include <libc/stdbool.h>
#include <fs/mbr.h>

#define ELF_MAGIC ("\x7F" "ELF")

typedef struct {
  uint8_t magic[4];
  uint8_t bits;             // 1 = 32, 2 = 64
  uint8_t endian;           // 1 = little, 2 = big
  uint8_t header_version;
  uint8_t abi;
  uint8_t _pad[8];
  uint16_t type;            // 1 = relocatable, 2 = executable, 3 = shared, 4 = core
  uint16_t instruction_set;
  uint32_t elf_version;
  uint32_t program_entry;
  uint32_t program_header_table;
  uint32_t section_header_table;
  uint32_t flags;
  uint16_t header_size;
  uint16_t program_header_entry_size;
  uint16_t program_header_entry_count;
  uint16_t section_header_entry_size;
  uint16_t section_header_entry_count;
  uint16_t section_names_index;
} __attribute__((packed)) elf_header_t;

typedef enum {
  ELF_BITNESS_32 = 1,
  ELF_BITNESS_64 = 2
} ELF_Bitness;

typedef enum {
  ELF_ENDIAN_LITTLE = 1,
  ELF_ENDIAN_BIG    = 2
} ELF_Endian;

typedef enum {
  ELF_TYPE_RELOCATABLE = 1,
  ELF_TYPE_EXECUTABLE  = 2,
  ELF_TYPE_SHARED      = 3,
  ELF_TYPE_CORE        = 4
} ELF_Type;

typedef enum {
  ELF_INST_SET_NONE   = 0x00,
  ELF_INST_SET_X86    = 0x03,
  ELF_INST_SET_ARM    = 0x28,
  ELF_INST_SET_X64    = 0x3E,
  ELF_INST_SET_ARM64  = 0xB7,
  ELF_INST_SET_RISCV  = 0xF3
} ELF_InstructionSet;

typedef struct {
  uint32_t type;
  uint32_t offset;
  uint32_t virtual_address;
  uint32_t physical_address;
  uint32_t file_size;
  uint32_t memory_size;
  uint32_t flags;
  uint32_t alignment;
} elf_program_header_t;

typedef enum {
  ELF_PH_TYPE_NULL    = 0x00,         // Unused
  ELF_PH_TYPE_LOAD    = 0x01,         // Loadable segment
  ELF_PH_TYPE_DYNAMIC = 0x02,         // Dynamic linking information
  ELF_PH_TYPE_INTERP  = 0x03,         // Interpreter information
  ELF_PH_TYPE_NOTE    = 0x04,         // Auxiliary information
  ELF_PH_TYPE_SHLIB   = 0x05,         // Reserved
  ELF_PH_TYPE_PHDR    = 0x06,         // Segment containing program header table itself
  ELF_PH_TYPE_TLS     = 0x07,         // Thread-local storage template
  ELF_PH_TYPE_LOOS    = 0x60000000,   // Environment-specific use (Operating System-specific)
  ELF_PH_TYPE_HIOS    = 0x6FFFFFFF,
  ELF_PH_TYPE_LOPROC  = 0x70000000,   // Processor-specific use (Processor-specific)
  ELF_PH_TYPE_HIPROC  = 0x7FFFFFFF
} ELF_PH_Type;

bool elf_read(partition_t* part, const char* path, void** entry_point);

#endif