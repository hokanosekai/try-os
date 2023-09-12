#ifndef __X86_H__
#define __X86_H__

#include <libc/stdint.h>
#include <libc/stdbool.h>

#define ASMCALL __attribute__((cdecl))

void ASMCALL x86_outb(uint16_t port, uint8_t value);
uint8_t ASMCALL x86_inb(uint16_t port);

bool ASMCALL x86_Disk_GetDriveParams(uint8_t drive,
                                     uint8_t* typeOut,
                                     uint16_t* cylindersOut,
                                     uint16_t* sectorsOut,
                                     uint16_t* headsOut);

bool ASMCALL x86_Disk_Reset(uint8_t drive);
bool ASMCALL x86_Disk_Read(uint8_t drive,
                           uint16_t cylinder,
                           uint16_t sector,
                           uint16_t head,
                           uint8_t count,
                           void* dataOut);

typedef struct {
  uint64_t base;
  uint64_t length;
  uint32_t type;
  uint32_t acpi;
} E820_memory_block_t;

typedef enum {
  E820_USABLE           = 1,
  E820_RESERVED         = 2,
  E820_ACPI_RECLAIMABLE = 3,
  E820_ACPI_NVS         = 4,
  E820_BAD_MEMORY       = 5,
} E820_Memory_Block_Type;

int ASMCALL x86_E820_GetNextMemoryBlock(E820_memory_block_t* blockOut, uint32_t* continuationId);