bits 16

section _TEXT class=CODE

;
; void _cdecl x86_div64_32(uint64_t dividend, uint32_t divisor, uint64_t* quotient, uint32_t* remainder);
;
; Inputs:
;   ax = dividend
;   dx = divisor
; Outputs:
;   ax = quotient
;   dx = remainder
global _x86_div64_32
_x86_div64_32:
  ; make new call frame
  push bp               ; save old call frame
  mov bp, sp            ; initialize new call frame

  push bx               ; save bx

  ; divide upper 32 bits
  mov eax, [bp + 8]     ; eax = dividend
  mov ecx, [bp + 12]    ; ecx = divisor
  xor edx, edx          ; edx = 0
  div ecx               ; eax = eax / ecx, edx = eax % ecx

  ; save quotient
  mov bx, [bp + 16]     ; bx = quotient
  mov [bx + 4], eax     ; [bx + 4] = eax

  ; divide lower 32 bits
  mov eax, [bp + 4]     ; eax = dividend
                        ; edx = old remainder
  div ecx               ; eax = eax / ecx, edx = eax % ecx

  ; save remainder
  mov [bx], eax         ; [bx] = eax
  mov bx, [bp + 18]     ; bx = quotient
  mov [bx], edx         ; [bx] = edx

  pop bx                ; restore bx

  ; restore old call frame
  mov sp, bp            ; restore stack pointer
  pop bp                ; restore base pointer
  ret                   ; return to caller

;
; void _cdecl x86_Video_ClearScreen(void);
;
; int 10h, ah=06h
; Inputs:
;   none
; Outputs:
;   none
global _x86_Video_ClearScreen
_x86_Video_ClearScreen:
  ; make new call frame
  push bp               ; save old call frame
  mov bp, sp            ; initialize new call frame

  ; save bx
  push bx

  ; clear screen
  mov ah, 06h           ; int 10h, ah=06h
  mov al, 0             ; al = blank character
  mov bh, 07h           ; bh = attribute
  mov cx, 0             ; cx = upper left corner
  mov dx, 184fh         ; dx = lower right corner
  int 10h               ; call BIOS video interrupt

  ; restore bx
  pop bx

  ; restore old call frame
  mov sp, bp            ; restore stack pointer
  pop bp                ; restore base pointer
  ret                   ; return to caller

;
; void _cdecl x86_Video_WriteCharTeletype(char c, uint8_t page);
;
; int 10h, ah=0eh
; Inputs:
;   al = character to write
;   bh = page number
; Outputs:
;   none
global _x86_Video_WriteCharTeletype
_x86_Video_WriteCharTeletype:
  ; make new call frame
  push bp               ; save old call frame
  mov bp, sp            ; initialize new call frame

  ; save bx
  push bx

  ; [bp + 0] = old call frame
  ; [bp + 2] = return address (small memory model => 2 bytes)
  ; [bp + 4] = al
  ; [bp + 6] = bh
  mov ah, 0eh           ; int 10h, ah=0eh
  mov al, [bp + 4]      ; al = character to write
  mov bh, [bp + 6]      ; bh = page number

  int 10h               ; call BIOS video interrupt

  ; restore bx
  pop bx

  ; restore old call frame
  mov sp, bp            ; restore stack pointer
  pop bp                ; restore base pointer
  ret                   ; return to caller

;
; char _cdecl x86_Video_ReadCharTeletype(uint8_t page);
;
; Inputs:
;   bh = page number
; Outputs:
;   al = character read
;   ah = scan code
global _x86_Video_ReadCharTeletype
_x86_Video_ReadCharTeletype:
  ; make new call frame
  push bp               ; save old call frame
  mov bp, sp            ; initialize new call frame

  ; save bx
  push bx

  ; [bp + 0] = old call frame
  ; [bp + 2] = return address (small memory model => 2 bytes)
  ; [bp + 4] = bh
  mov ax, 0             ; int 16h, ah=0
  int 16h               ; call BIOS keyboard interrupt

  mov ah, 0x0e          ; int 10h, ah=0eh
  mov bh, [bp + 4]      ; bh = page number

  int 10h               ; call BIOS video interrupt

  ; restore bx
  pop bx

  ; restore old call frame
  mov sp, bp            ; restore stack pointer
  pop bp                ; restore base pointer
  ret                   ; return to caller