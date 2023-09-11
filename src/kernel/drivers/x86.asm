extern LoadGDT

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
  call LoadGDT

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
  mov ax, 0800h         ; int 13h, ah=08h
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

global x86_Video_GetVbeInfo
x86_Video_GetVbeInfo:
  ; make new call frame
  push ebp              ; save old call frame
  mov ebp, esp          ; initialize new call frame

  x86_EnterRealMode

  ; save registers
  push edi              ; save edi
  push es               ; save es
  push ebp              ; save ebp (Note: bochs vbe change ebp)

  ; call BIOS video interrupt
  mov ax, 0x4f00        ; int 10h, ax=4f00h (0x4f00 = get vbe info)
  LinearToSegOffset [bp + 8], es, edi, di
  int 10h               ; call BIOS video interrupt

  ; check return
  cmp al, 4fh           ; al == 0x4f?
  jne .error            ; no, error

  ; put status in eax
  mov al, ah            ; al = ah (status)
  and eax, 0xff         ; eax = eax & 0xff (status)
  jmp .cont             ; continue

.error:
  mov eax, -1           ; eax = -1 (error)

.cont:
  ; restore registers
  pop ebp               ; restore ebp
  pop es                ; restore es
  pop ebx               ; restore ebx

  push eax              ; save eax

  x86_EnterProtectedMode

  pop eax               ; restore eax

  ; restore old call frame
  mov esp, ebp          ; restore stack pointer
  pop ebp               ; restore base pointer
  ret                   ; return to caller

global x86_Video_GetModeInfo
x86_Video_GetModeInfo:
  ; make new call frame
  push ebp              ; save old call frame
  mov ebp, esp          ; initialize new call frame

  x86_EnterRealMode

  ; save registers
  push edi              ; save edi
  push es               ; save es
  push ebp              ; save ebp (Note: bochs vbe change ebp)
  push ecx              ; save ecx

  ; call BIOS video interrupt
  mov ax, 0x4f01        ; int 10h, ax=4f01h (0x4f01 = get mode info)
  mov cx, [bp + 8]      ; cx = mode number
  LinearToSegOffset [bp + 12], es, edi, di
  int 10h               ; call BIOS video interrupt

  ; check return
  cmp al, 4fh           ; al == 0x4f?
  jne .error            ; no, error

  ; put status in eax
  mov al, ah            ; al = ah (status)
  and eax, 0xff         ; eax = eax & 0xff (status)
  jmp .cont             ; continue

.error:
  mov eax, -1           ; eax = -1 (error)

.cont:
  ; restore registers
  pop ecx               ; restore ecx
  pop ebp               ; restore ebp
  pop es                ; restore es
  pop ebx               ; restore ebx

  push eax              ; save eax

  x86_EnterProtectedMode

  pop eax               ; restore eax

  ; restore old call frame
  mov esp, ebp          ; restore stack pointer
  pop ebp               ; restore base pointer
  ret                   ; return to caller

global x86_Video_SetMode
x86_Video_SetMode:
  ; make new call frame
  push ebp              ; save old call frame
  mov ebp, esp          ; initialize new call frame

  x86_EnterRealMode

  ; save registers
  push edi              ; save edi
  push es               ; save es
  push ebp              ; save ebp (Note: bochs vbe change ebp)
  push ebx              ; save ebx

  ; call BIOS video interrupt
  mov ax, 0             ; int 10h, ax=0 (0x00 = set mode)
  mov es, ax            ; es = ax (es = 0)
  mov edi, 0            ; edi = 0
  mov ax, 4f02h         ; int 10h, ax=4f02h (0x4f02 = set mode)
  mov bx, [bp + 8]      ; bx = mode number
  int 10h               ; call BIOS video interrupt

  ; check return
  cmp al, 4fh           ; al == 0x4f?
  jne .error            ; no, error

  ; put status in eax
  mov al, ah            ; al = ah (status)
  and eax, 0ffh         ; eax = eax & 0xff (status)
  jmp .cont             ; continue

.error:
  mov eax, -1           ; eax = -1 (error)

.cont:
  ; restore registers
  pop ebx               ; restore ebx
  pop ebp               ; restore ebp
  pop es                ; restore es
  pop ebx               ; restore ebx

  push eax              ; save eax

  x86_EnterProtectedMode

  pop eax               ; restore eax

  ; restore old call frame
  mov esp, ebp          ; restore stack pointer
  pop ebp               ; restore base pointer
  ret                   ; return to caller

global x86_PIT_GetTickCount
x86_PIT_GetTickCount:
  ; make new call frame
  push ebp              ; save old call frame
  mov ebp, esp          ; initialize new call frame

  x86_EnterRealMode

  ; save registers
  push edi              ; save edi
  push es               ; save es
  push ebp              ; save ebp (Note: bochs vbe change ebp)

  ; call BIOS timer interrupt
  mov ah, 2ch             ; int 1ah, ax=0 (0x00 = get tick)
  int 21h               ; call BIOS timer interrupt

  ; put tick in eax
  mov eax, ecx          ; eax = ecx (tick)

  ; restore registers
  pop ebp               ; restore ebp
  pop es                ; restore es
  pop ebx               ; restore ebx

  push eax              ; save eax

  x86_EnterProtectedMode

  pop eax               ; restore eax

  ; restore old call frame
  mov esp, ebp          ; restore stack pointer
  pop ebp               ; restore base pointer
  ret                   ; return to caller

global x86_PIT_GetHours
x86_PIT_GetHours:
  ; make new call frame
  push ebp              ; save old call frame
  mov ebp, esp          ; initialize new call frame

  x86_EnterRealMode

  ; save registers
  push edi              ; save edi
  push es               ; save es
  push ebp              ; save ebp (Note: bochs vbe change ebp)

  ; --- Insert RTC Reading Code Here ---
  ; Disable NMI (Non-Maskable Interrupts) to prevent RTC updates
  cli

  ; Select RTC register A (0x0A) to disable RTC updates during read
  mov al, 0x0A
  out 0x70, al

  ; Read RTC register A
  in al, 0x71

  ; Enable NMI
  sti

  ; Check if the RTC update-in-progress (UIP) flag is set
  test al, 0x80
  jnz _retry  ; If UIP is set, wait and check again

rtc_read_loop:
  ; Read hours (RTC register 0x04)
  mov al, 0x00
  out 0x70, al
  in al, 0x71
  mov eax, edx  ; Store hours in eax

  ; --- End of RTC Reading Code ---

_retry:
  ; call BIOS timer interrupt
  mov ah, 2Ch             ; int 1Ah, ax=0 (0x00 = get tick)
  int 21h               ; call BIOS timer interrupt


  ; restore registers
  pop eax               ; restore eax
  pop es                ; restore es
  pop edi               ; restore edi

  x86_EnterProtectedMode

  ; restore old call frame
  mov esp, ebp          ; restore stack pointer
  pop ebp               ; restore base pointer
  ret                   ; return to caller