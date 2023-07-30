org 0x7C00            ; BIOS loads boot sector to 0x7C00
bits 16               ; 16-bit code

; define end of line macro
%define ENDL 0x0D 0x0A

start:
  jmp main

; Print a string to the screen
; Inputs:
;   si - pointer to string
puts:
  push si             ; save si
  push ax             ; save ax
  push bx             ; save bx

; loop through string until null terminator
.loop:
  ; load next character
  lodsb               ; load byte from si into al, increment si
  or al, al           ; set flags based on al
  jz .done            ; if al is zero, we're done (no more characters)

  ; call BIOS interrupt to print character
  mov ah, 0x0E        ; teletype output
  mov bh, 0           ; page number
  int 0x10            ; call BIOS interrupt

  jmp .loop

; return from puts
.done:
  pop bx              ; restore bx
  pop ax              ; restore ax
  pop si              ; restore si
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

  ; print message
  mov si, msg_hello   ; si = msg_hello
  call puts           ; call puts(msg_hello)

  hlt                 ; halt the CPU (infinite loop)

; infinite loop
.halt:
  jmp .halt

msg_hello:
  db "Hello, World!", ENDL, 0

times 510-($-$$) db 0 ; pad with zeros until 510 bytes
dw 0AA55h             ; boot signature
