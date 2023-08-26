bits 16

section .entry

extern __bss_start
extern __end

extern start
global entry

entry:
  cli

  ; save boot drive
  mov [g_BootDrive], dl

  ; setup stack
  mov ax, ds
  mov ss, ax
  mov sp, 0xFFF0
  mov bp, sp

  ; switch to 32-bit protected mode
  call EnableA20              ; enable A20 line
  call LoadGDT                ; load GDT

  ; set protection enable flag in CR0
  mov eax, cr0                ; set PE bit in CR0
  or al, 1
  mov cr0, eax

  ; far jump into protected mode
  jmp dword 08h:.pmode


.pmode:
  ; Here we are in 32-bit protected mode
  [bits 32]

  ; setup registers
  mov ax, 10h
  mov ds, ax
  mov ss, ax

  ; clear bss
  mov edi, __bss_start
  mov ecx, __end
  sub ecx, edi
  mov al, 0
  cld
  rep stosb

  ;expect boot drive in dl, then send it as argument to cstart function
  xor edx, edx
  mov dl, [g_BootDrive]
  push edx
  call start

  cli
  hlt

EnableA20:
  [bits 16]

  ; disable Keyboard
  call A20WaitInput
  mov al, KbdControllerDisableKeyboard
  out KbdControllerCommandPort, al

  ; read controller status
  call A20WaitInput
  mov al, KbdControllerReadStatus
  out KbdControllerCommandPort, al

  ; wait for input buffer to be clear
  call A20WaitOutput
  in al, KbdControllerDataPort
  push eax

  ; write controller command
  call A20WaitInput
  mov al, KbdControllerWriteStatus
  out KbdControllerCommandPort, al

  call A20WaitInput
  pop eax
  or al, 2
  out KbdControllerDataPort, al

  ; enable Keyboard
  call A20WaitInput
  mov al, KbdControllerEnableKeyboard
  out KbdControllerCommandPort, al

  call A20WaitInput
  ret

A20WaitInput:
  [bits 16]
  ; wait until status bit 2 is 0
  ; by reading from command port we are reading status port
  in al, KbdControllerCommandPort
  test al, 2
  jnz A20WaitInput
  ret

A20WaitOutput:
  [bits 16]
  ; wait until status bit 1 is 1
  ; so we can read from data port
  in al, KbdControllerCommandPort
  test al, 1
  jz A20WaitOutput
  ret

global LoadGDT
LoadGDT:
  [bits 16]
  lgdt [g_GDTDescriptor]     ; load GDT
  ret

KbdControllerDataPort         equ 0x60
KbdControllerCommandPort      equ 0x64
KbdControllerDisableKeyboard  equ 0xAD
KbdControllerEnableKeyboard   equ 0xAE
KbdControllerReadStatus       equ 0xD0
KbdControllerWriteStatus      equ 0xD1

ScreenBuffer                  equ 0xB8000

g_GDT:      ; NULL descriptor
            dq 0

            ; 32-bit code segment
            dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF for full 32-bit range
            dw 0                        ; base (bits 0-15) = 0x0
            db 0                        ; base (bits 16-23)
            db 10011010b                ; access (present, ring 0, code segment, executable, direction 0, readable)
            db 11001111b                ; granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
            db 0                        ; base high

            ; 32-bit data segment
            dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF for full 32-bit range
            dw 0                        ; base (bits 0-15) = 0x0
            db 0                        ; base (bits 16-23)
            db 10010010b                ; access (present, ring 0, data segment, executable, direction 0, writable)
            db 11001111b                ; granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
            db 0                        ; base high

            ; 16-bit code segment
            dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF
            dw 0                        ; base (bits 0-15) = 0x0
            db 0                        ; base (bits 16-23)
            db 10011010b                ; access (present, ring 0, code segment, executable, direction 0, readable)
            db 00001111b                ; granularity (1b pages, 16-bit pmode) + limit (bits 16-19)
            db 0                        ; base high

            ; 16-bit data segment
            dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF
            dw 0                        ; base (bits 0-15) = 0x0
            db 0                        ; base (bits 16-23)
            db 10010010b                ; access (present, ring 0, data segment, executable, direction 0, writable)
            db 00001111b                ; granularity (1b pages, 16-bit pmode) + limit (bits 16-19)
            db 0                        ; base high

g_GDTDescriptor:  dw g_GDTDescriptor - g_GDT - 1    ; limit = size of GDT
                  dd g_GDT                          ; address of GDT

g_BootDrive: db 0
