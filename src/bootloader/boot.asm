org 0x7C00                          ; BIOS loads boot sector to 0x7C00
bits 16                             ; 16-bit code

; define end of line macro
%define ENDL 0x0D, 0x0A

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
bdb_media_descriptor_type:  db 0F0h                 ; media descriptor type (0F0h = 1.44MB 3.5' floppy)
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
  ; setup data segments
  mov ax, 0
  mov ds, ax
  mov es, ax

  ; setup stack
  mov ss, ax
  mov sp, 0x7C00

  ; check if the BIOS loaded us to 0000:7C00
  push es
  push word .after
  retf                              ; return far to 0000:7C00

.after:
  ; read something from the floppy disk
  ; BIOS should set dl to drive number
  mov [ebr_drive_number], dl        ; dl = drive number

  ; print loading message
  mov si, msg_loading               ; si = msg_loading
  call puts                         ; call puts(msg_loading)

  ; read drive parameters (sectors per track, heads, etc.)
  push es                           ; save es
  mov ah, 08h                       ; get drive parameters
  int 13h                           ; call BIOS interrupt
  jc floppy_error                   ; if carry flag is set, jump to floppy_error
  pop es                            ; restore es

  and cl, 0x3F                      ; clear bits 6-7 of cl (sectors per track)
  xor ch, ch                        ; clear ch (cylinder number)
  mov [bdb_sectors_per_track], cx   ; cx = sectors count

  inc dh                            ; increment dh (head number)
  mov [bdb_heads], dh               ; dh = head count

  ; compute LBA of root directory (reserved + FATs * sectors per FAT)
  ; NOTE: can be hardcoded
  mov ax, [bdb_sectors_per_fat]     ; ax = sectors per FAT
  mov bl, [bdb_fat_count]           ; bl = FAT count
  xor bh, bh                        ; clear bh
  mul bx                            ; ax = ax * bx (sectors per FAT * FAT count)
  add ax, [bdb_reserved_sectors]    ; ax = ax + reserved sectors (LBA of root directory)
  push ax                           ; save ax

  ; compute size of root directory in sectors 
  ; (number of entries * 32 bytes per entry / 512 bytes per sector)
  mov ax, [bdb_dir_entries_count]   ; ax = number of entries
  shl ax, 5                         ; ax = ax * 32 (number of entries * 32 bytes per entry)
  xor dx, dx                        ; clear dx
  div word [bdb_bytes_per_sector]   ; ax = ax / 512

  test dx, dx                       ; test dx (if dx != 0, add 1)
  jz .root_dir_after                ; if dx == 0, jump to .root_dir_after
  inc ax                            ; ax = ax + 1
                                    ; NOTE: this means we have a sector that is not full

.root_dir_after:
  ; read root directory
  mov cl, al                        ; cl = al (number of sectors to read)
  pop ax                            ; ax = LBA of root directory
  mov dl, [ebr_drive_number]        ; dl = drive number
  mov bx, buffer                    ; es:bx = buffer
  call read_disk                    ; call read_disk(LBA of root directory, number of sectors to read, buffer)

  ; search for stage2 file
  xor bx, bx                        ; bx = 0 (index of root directory entry)
  mov di, buffer                    ; di = buffer

.search_kernel:
  mov si, kernel_filename           ; si = kernel_filename
  mov cx, 11                        ; cx = kernel_filename_len
  push di                           ; save di
  repe cmpsb                        ; compare si and di, decrement cx
  pop di                            ; restore di
  je .kernel_found                  ; if cx == 0, jump to .kernel_found

  ; if no stage2 found, we check the next entry
  add di, 32                        ; di = di + 32 (next entry)
  inc bx                            ; bx = bx + 1 (next entry)
  cmp bx, [bdb_dir_entries_count]   ; compare bx and number of entries
  jl .search_kernel                 ; if bx < number of entries, jump to .search_kernel

  ; if stage2 not found, print error message and halt
  jmp kernel_not_found              ; jump to kernel_not_found

.kernel_found:
  ; di points to stage2 file entry
  mov ax, [di + 26]                 ; ax = di + 26 (LBA of stage2 file)
  mov [kernel_cluster], ax          ; kernel_cluster = LBA of stage2 file

  ; load FAT from disk to memory
  mov ax, [bdb_reserved_sectors]    ; ax = reserved sectors
  mov bx, buffer                    ; es:bx = buffer
  mov cl, [bdb_sectors_per_fat]     ; cl = sectors per FAT
  mov dl, [ebr_drive_number]        ; dl = drive number
  call read_disk                    ; call read_disk(reserved sectors, sectors per FAT, buffer)

  ; load stage2 from disk to memory
  mov bx, KERNEL_LOAD_SEGMENT       ; es:bx = KERNEL_LOAD_SEGMENT
  mov es, bx                        ; es = bx
  mov bx, KERNEL_LOAD_OFFSET        ; bx = KERNEL_LOAD_OFFSET

.load_kernel_loop:
  ; read next cluster
  mov ax, [kernel_cluster]          ; ax = kernel_cluster

  add ax, 31                        ; ax = ax + 31

  mov cl, 1                         ; cl = 1
  mov dl, [ebr_drive_number]        ; dl = drive number
  call read_disk                    ; call read_disk(kernel_cluster, 1, es:bx)

  add bx, [bdb_bytes_per_sector]    ; bx = bx + bytes per sector

  ; compute next cluster
  mov ax, [kernel_cluster]          ; ax = kernel_cluster
  mov cx, 3                         ; cx = 3
  mul cx                            ; ax = ax * cx
  mov cx, 2                         ; cx = 2
  div cx                            ; ax = ax / cx (index of FAT entry)

  mov si, buffer                    ; si = buffer
  add si, ax                        ; si = si + ax (address of FAT entry)
  mov ax, [ds:si]                   ; ax = FAT entry

  or dx, dx                         ; set flags based on dx
  jz .even                          ; if dx == 0, jump to .even

.odd:
  shr ax, 4                         ; ax = ax >> 4 (next cluster)
  jmp .next_cluster

.even:
  and ax, 0x0FFF                    ; ax = ax & 0x0FFF (next cluster)

.next_cluster:
  cmp ax, 0x0FF8                    ; compare ax and 0x0FF8
  jae .read_done                    ; if ax >= 0x0FF8, jump to .read_done

  mov [kernel_cluster], ax          ; kernel_cluster = next cluster
  jmp .load_kernel_loop             ; jump to .load_kernel_loop

.read_done:
  ; jump to stage2
  mov dl, [ebr_drive_number]        ; dl = drive number (boot drive)

  mov ax, KERNEL_LOAD_SEGMENT       ; ax = KERNEL_LOAD_SEGMENT
  mov ds, ax                        ; ds = ax
  mov es, ax                        ; es = ax

  jmp KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET

  jmp wait_for_keypress             ; jump to wait_for_keypress

  cli                               ; clear interrupts
  hlt                               ; halt

; Error handling
floppy_error:
  mov si, msg_read_error            ; si = msg_read_error
  call puts                         ; call puts(msg_read_error)
  jmp wait_for_keypress             ; jump to wait_for_keypress

kernel_not_found:
  mov si, msg_kernel_not_found      ; si = msg_kernel_not_found
  call puts                         ; call puts(msg_kernel_not_found)
  jmp wait_for_keypress             ; jump to wait_for_keypress

; wait for keypress
wait_for_keypress:
  mov ah, 0                         ; get keystroke
  int 16h                           ; call BIOS interrupt
  jmp 0FFFFh:0                      ; jump to 0x0000:0xFFFF (should reboot)

.halt:
  cli                               ; disable interrupts
  hlt                               ; halt the CPU

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

; Convert a logical block address to a CHS address
; Inputs:
;   ax - logical block address
; Outputs:
;   cx [bits 0-5] - sector number
;   cx [bits 6-15] - cylinder number
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
read_disk:
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

  ; error occurred, print error message
  popa                              ; restore registers
  call reset_disk                   ; reset disk

  ; retry
  dec di                            ; decrement retry count
  test di, di                       ; test if retry count is zero
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
reset_disk:
  pusha                             ; save registers
  mov ah, 0                         ; reset disk
  stc                               ; set carry flag
  int 13h                           ; call BIOS interrupt
  jc floppy_error                   ; if error, jump to floppy_error
  popa                              ; restore registers
  ret

msg_loading:            db 'Loading', ENDL, 0
msg_kernel_not_found:   db 'KERNEL.BIN not found!', ENDL, 0
msg_read_error:         db 'Error reading disk!', ENDL, 0

kernel_filename:        db 'KERNEL  BIN'
kernel_cluster:         dw 0

KERNEL_LOAD_SEGMENT     equ 0x0
KERNEL_LOAD_OFFSET      equ 0x500

times 510-($-$$) db 0               ; pad with zeros until 510 bytes
dw 0AA55h                           ; boot signature

buffer: