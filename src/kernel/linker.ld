ENTRY(start)
OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
phys = 0x00000500;

SECTIONS
{
  . = phys;

  .entry    : { __entry_start   = .;  *(.entry) }
  .text     : { __text_start    = .;  *(.text)  }
  .data     : { __data_start    = .;  *(.data)  }
  .rodata   : { __rodata_start  = .;  *(.rodata)}
  .bss      : { __bss_start     = .;  *(.bss)   }

  __end = .;
}