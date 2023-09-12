%macro x86_EnterRealMode 0
  [bits 32]
  jmp word 18h:.pmode16

.pmode16:
  [bits 16]
  ; disable protected mode bit in cr0
  mov eax, cr0           ; eax = cr0
  and al, ~1             ; al = cr0 & ~1 (clear bit 0)
  mov cr0, eax           ; cr0 = eax

  ; load real mode data segment registers
  jmp word 00h:.rmode

.rmode:
  ; setup segment registers
  mov ax, 0
  mov ds, ax
  mov ss, ax

  ; enable interrupts
  sti

%endmacro

%macro x86_EnterProtectedMode 0
  cli

  ; enable protected mode bit in cr0
  mov eax, cr0           ; eax = cr0
  or al, 1               ; al = cr0 | 1 (set bit 0)
  mov cr0, eax           ; cr0 = eax

  ; far jump into protected mode
  jmp dword 08h:.pmode

.pmode:
  ; we are now in protected mode
  [bits 32]

  ; setup segment registers
  mov ax, 10h
  mov ds, ax
  mov ss, ax

%endmacro

;
; Convert linear address to segment:offset address
; Args:
;    1 - linear address
;    2 - (out) target segment (e.g. es)
;    3 - target 32-bit register to use (e.g. eax)
;    4 - target lower 16-bit half of #3 (e.g. ax)
;
%macro LinearToSegOffset 4

    mov %3, %1      ; linear address to eax
    shr %3, 4
    mov %2, %4
    mov %3, %1      ; linear address to eax
    and %3, 0xf

%endmacro

global x86_outb
x86_outb:
  [bits 32]
  mov dx, [esp + 4]    ; dx = port
  mov al, [esp + 8]    ; al = value
  out dx, al           ; outb dx, al
  ret

global x86_inb
x86_inb:
  [bits 32]
  mov dx, [esp + 4]    ; dx = port
  xor eax, eax         ; eax = 0
  in al, dx            ; inb dx, al
  ret

; x86_Disk_GetDriveParams
; Args:
;    1 - drive number
;    2 - (out) drive type
;    3 - (out) cylinders
;    4 - (out) sectors
;    5 - (out) heads
global x86_Disk_GetDriveParams
x86_Disk_GetDriveParams:
  [bits 32]

  ; make new call frame
  push ebp              ; save old call frame
  mov ebp, esp          ; initialize new call frame

  x86_EnterRealMode

  [bits 16]

  ; save registers
  push es
  push bx
  push esi
  push di

  mov dl, [bp + 8]      ; dl = drive number
  mov ax, 08h           ; int 13h, ah=08h
  mov di, 0             ; di = 0
  mov es, di            ; es = 0
  stc                   ; set carry flag
  int 13h               ; call BIOS disk interrupt

  ; output parameters
  mov eax, 1            ; eax = 1
  sbb eax, 0            ; eax = 0 if carry flag is set

  LinearToSegOffset [bp + 12], es, esi, si
  mov [es:si], bl       ; store drive type in drive type

  ; cylindar count
  mov bl, ch            ; bl = ch (lower 8 bits of cylindar count)
  mov bh, cl            ; bh = cl (upper 2 bits of cylindar count)
  shr bh, 6             ; bh = bh >> 6 (upper 2 bits of cylindar count)
  inc bx                ; bx = bx + 1 (cylindar count)

  LinearToSegOffset [bp + 16], es, esi, si
  mov [es:si], bx       ; store cylindar count in cylindar count

  ; sector count
  xor ch, ch            ; ch = 0 (lower 5 bits in cl)
  and cl, 3fh           ; cl = cl & 3fh (lower 6 bits in cl)

  LinearToSegOffset [bp + 20], es, esi, si
  mov [es:si], cx       ; store sector count in sector count

  ; head count
  mov cl, dh            ; cl = dh (lower 8 bits of head count)
  inc cx                ; cx = cx + 1 (head count)

  LinearToSegOffset [bp + 24], es, esi, si
  mov [es:si], cx       ; store head count in head count

  ; restore registers
  pop di
  pop esi
  pop bx
  pop es

  push eax              ; save eax

  x86_EnterProtectedMode

  [bits 32]

  pop eax               ; restore eax

  ; restore old call frame
  mov esp, ebp          ; restore stack pointer
  pop ebp               ; restore base pointer
  ret                   ; return to caller

global x86_Disk_Reset
x86_Disk_Reset:
  [bits 32]

  ; make new call frame
  push ebp              ; save old call frame
  mov ebp, esp          ; initialize new call frame

  x86_EnterRealMode

  mov ah, 0             ; int 13h, ah=0
  mov dl, [bp + 8]      ; dl = drive number
  stc                   ; set carry flag
  int 13h               ; call BIOS disk interrupt

  mov eax, 1            ; eax = 1
  sbb eax, 0            ; eax = 0 if carry flag is set

  push eax              ; save eax

  x86_EnterProtectedMode

  pop eax               ; restore eax

  ; restore old call frame
  mov esp, ebp          ; restore stack pointer
  pop ebp               ; restore base pointer
  ret                   ; return to caller

global x86_Disk_Read
x86_Disk_Read:
  ; make new call frame
  push ebp              ; save old call frame
  mov ebp, esp          ; initialize new call frame

  x86_EnterRealMode

  ; save registers
  push ebx              ; save ebx
  push es               ; save es

  ; setup args
  mov dl, [bp + 8]      ; dl = drive number
  mov ch, [bp + 12]     ; ch = cylinder (lower 8 bits)
  mov cl, [bp + 13]     ; cl = cylinder (2 bits 6-7)
  shl cl, 6             ; cl = cl << 6

  mov al, [bp + 16]     ; al = sector count 
  and al, 3fh           ; al = al & 3fh (bits 0-5)
  or cl, al             ; cl = cl | al (bits 0-5)

  mov dh, [bp + 20]     ; dh = head
  mov al, [bp + 24]     ; al = sector

  LinearToSegOffset [bp + 28], es, ebx, bx

  ; call BIOS disk interrupt
  mov ah, 02h           ; int 13h, ah=02h (0x02 = read sectors)
  stc                   ; set carry flag
  int 13h               ; call BIOS disk interrupt

  ; restore registers
  pop es                ; restore es
  pop ebx               ; restore ebx

  push eax              ; save eax

  x86_EnterProtectedMode

  pop eax               ; restore eax

  ; restore old call frame
  mov esp, ebp          ; restore stack pointer
  pop ebp               ; restore base pointer
  ret                   ; return to caller

; int ASMCALL x86_E820_GetNextMemoryBlock(E820_memory_block_t* blockOut, uint32_t* continuationId);
E820Signature equ 0x534D4150

global x86_E820_GetNextMemoryBlock
x86_E820_GetNextMemoryBlock:
  ; make new call frame
  push ebp              ; save old call frame
  mov ebp, esp          ; initialize new call frame

  x86_EnterRealMode

  ; save registers
  push ebx              ; save ebx
  push ecx              ; save ecx
  push edx              ; save edx
  push esi              ; save esi
  push edi              ; save edi
  push ds               ; save ds
  push es               ; save es

  ; setup params
  LinearToSegOffset [bp + 8], es, esi, di     ; es:di = ptr to struct

  LinearToSegOffset [bp + 12], ds, esi, si    ; ebx = ptr to continuation id
  mov ebx, ds:[si]

  mov eax, 0e820h         ; int 15h, eax=0e820h
  mov edx, E820Signature  ; edx = E820Signature
  mov ecx, 24             ; ecx = 24 (size of struct)

  ; call BIOS memory interrupt
  int 15h                 ; call BIOS memory interrupt

  ; test 
  cmp eax, E820Signature  ; eax == E820Signature?
  jne .error              ; if not, jump to error

.if_success:
  mov eax, ecx            ; eax = ecx (return value)
  mov ds:[si], ebx        ; *ebx = continuation id
  jmp .done

.error:
  mov eax, -1             ; eax = -1 (return value)

.done:
  ; restore registers
  pop es                  ; restore es
  pop ds                  ; restore ds
  pop edi                 ; restore edi
  pop esi                 ; restore esi
  pop edx                 ; restore edx
  pop ecx                 ; restore ecx
  pop ebx                 ; restore ebx

  push eax                ; save eax

  x86_EnterProtectedMode

  pop eax                 ; restore eax

  ; restore old call frame
  mov esp, ebp            ; restore stack pointer
  pop ebp                 ; restore base pointer
  ret                     ; return to caller