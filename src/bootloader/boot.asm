org 0x7C00                          ; BIOS loads boot sector to 0x7C00
bits 16                             ; 16-bit code

; define null terminator macro
%define NULL 0x00
; define carriage return macro
%define CR 0x0D
; define line feed macro
%define LF 0x0A
; define end of line macro
%define ENDL CR, LF
; define line break macro
%define BR ENDL, NULL

; FAT12 header
jmp short start                     ; jump to start of code
nop                                 ; pad with a nop

; BIOS Parameter Block
bdb_oem:                    db 'MSWIN4.1'           ; OEM name (8 bytes)
bdb_bytes_per_sector:       dw 512                  ; bytes per sector (1024)
bdb_sectors_per_cluster:    db 1                    ; sectors per cluster
bdb_reserved_sectors:       dw 1                    ; number of reserved sectors
bdb_fat_count:              db 2                    ; number of FATs
bdb_dir_entries_count:      dw 0E0h                 ; number of directory entries (224)
bdb_total_sectors:          dw 2880                 ; total number of sectors (1024 * 1440 = 1.44MB)
bdb_media_descriptor_type:  db 0F0h                 ; media descriptor type (0F0h = 1.44MB 3.5" floppy)
bdb_sectors_per_fat:        dw 9                    ; sectors per FAT
bdb_sectors_per_track:      dw 18                   ; sectors per track
bdb_heads:                  dw 2                    ; number of heads
bdb_hidden_sectors:         dd 0                    ; number of hidden sectors
bdb_large_sector_count:     dd 0                    ; large total number of sectors

; Extended BIOS Parameter Block
ebr_drive_number:           db 0                    ; drive number (0x00 = floppy, 0x80 = hard disk)
                            db 0                    ; reserved
ebr_signature:              db 29h                  ; extended boot signature (29h)
ebr_volume_id:              dd 13h, 37h, 13h, 37h   ; volume ID
ebr_volume_label:           db 'TRY OS     '        ; volume label (11 bytes)
ebr_system_id:              db 'FAT12   '           ; system ID (8 bytes)

start:
  jmp main

; Print a string to the screen
; Inputs:
;   si - pointer to string
puts:
  push si                           ; save si
  push ax                           ; save ax
  push bx                           ; save bx

; loop through string until null terminator
.loop:
  ; load next character
  lodsb                             ; load byte from si into al, increment si
  or al, al                         ; set flags based on al
  jz .done                          ; if al is zero, we're done (no more characters)

  ; call BIOS interrupt to print character
  mov ah, 0x0E                      ; teletype output
  mov bh, 0                         ; page number
  int 0x10                          ; call BIOS interrupt

  jmp .loop

; return from puts
.done:
  pop bx                            ; restore bx
  pop ax                            ; restore ax
  pop si                            ; restore si
  ret

; main program
main:
  ; setup data segments
  mov ax, 0
  mov ds, ax
  mov es, ax

  ; setup stack
  mov ss, ax
  mov sp, 0x7C00

  ; read something from disk
  ; BIOS should set dl to drive number
  mov [ebr_drive_number], dl        ; dl = drive number

  mov ax, 1                         ; ax = 1 (second sector)
  mov cl, 1                         ; cl = 1 (read 1 sector)
  mov bx, 0x7E00                    ; es:bx = 0x0000:0x7E00 (buffer to read into)
  call disk_read                    ; call disk_read

  ; print message
  mov si, msg_hello                 ; si = msg_hello
  call puts                         ; call puts(msg_hello)

  cli                               ; disable interrupts
  hlt                               ; halt the CPU

; Error handling
floppy_error:
  mov si, msg_read_error            ; si = msg_read_error
  call puts                         ; call puts(msg_read_error)
  jmp wait_for_keypress             ; jump to wait_for_keypress

; wait for keypress
wait_for_keypress:
  mov si, msg_press_any_key         ; si = msg_press_any_key
  call puts                         ; call puts(msg_press_any_key)

  mov ah, 0x00                      ; get keystroke
  int 0x16                          ; call BIOS interrupt
  jmp 0FFFFh:0                      ; jump to 0x0000:0xFFFF (should reboot)

.halt:
  cli                               ; disable interrupts
  hlt                               ; halt the CPU

; Convert a logical block address to a CHS address
; Inputs:
;   ax - logical block address
; Outputs:
;   cx [bits 0-5] - sector number
;   ch [bits 6-15] - cylinder number
;   dh - head number
lba_to_chs:
  push ax                           ; save ax
  push dx                           ; save dx

  xor dx, dx                        ; clear dx
  div word [bdb_sectors_per_track]  ; ax = LBA / sectors_per_track
                                    ; dx = LBA % sectors_per_track

  inc dx                            ; dx = (LBA % sectors_per_track + 1) = sector
  mov cx, dx                        ; cx = sector

  xor dx, dx                        ; clear dx
  div word [bdb_heads]              ; ax = (LBA / sectors_per_track) / heads = cylinder
                                    ; dx = (LBA / sectors_per_track) % heads = head
  mov dh, dl                        ; dh = head
  mov ch, al                        ; ch = cylinder (lower 8 bits)
  shl ah, 6                         ; ah = cylinder (upper 2 bits)
  or cl, ah                         ; put upper 2 bits of cylinder into cl

  pop ax                            ; restore ax
  mov dl, al                        ; restore dl
  pop ax                            ; restore ax
  ret

; Read a sector from disk
; Inputs:
;   ax - logical block address
;   cl - number of sectors to read
;   dl - drive number
;   es:bx - buffer to read into
disk_read:
  push ax                           ; save ax
  push bx                           ; save bx
  push cx                           ; save cx
  push dx                           ; save dx
  push di                           ; save di

  push cx                           ; save cx temporarily (number of sectors to read)
  call lba_to_chs                   ; convert LBA to CHS
  pop ax                            ; al = number of sectors to read

  mov ah, 02h                       ; read sector
  mov di, 3                         ; retry count


.retry:
  pusha                             ; save registers
  stc                               ; set carry flag
  int 13h                           ; call BIOS interrupt
  jnc .done                         ; if no error, we're done

  ; print retry error message
  mov si, msg_retry_error           ; si = msg_retry_error
  call puts                         ; call puts(msg_retry_error)

  ; error occurred, print error message
  popa                              ; restore registers
  call disk_reset                   ; reset disk

  ; retry
  dec di                            ; decrement retry count
  jnz .retry                        ; if retry count is not zero, try again


.fail:
  jmp floppy_error                  ; jump to floppy error

.done:
  popa                              ; restore registers
  pop di                            ; restore di
  pop dx                            ; restore dx
  pop cx                            ; restore cx
  pop bx                            ; restore bx
  pop ax                            ; restore ax
  ret

; Reset the disk
; Inputs:
;   dl - drive number
disk_reset:
  pusha                             ; save registers
  mov ah, 00h                       ; reset disk
  stc                               ; set carry flag
  int 13h                           ; call BIOS interrupt
  jc floppy_error                   ; if error, jump to floppy_error
  popa                              ; restore registers
  ret

msg_hello:            db "Hello, World!", ENDL, 0
msg_press_any_key:    db "Press any key to reboot...", ENDL, 0
msg_retry_error:      db "Error reading disk! Retrying...", ENDL, 0
msg_read_error:       db "Error reading disk!", ENDL, 0

times 510-($-$$) db 0               ; pad with zeros until 510 bytes
dw 0AA55h                           ; boot signature
