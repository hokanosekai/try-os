[bits 32]

; int __attribute__((cdecl)) GDT_load(GDT_Descriptor* descriptor, uint16_t codeSegment, uint16_t dataSegment)
global GDT_load
GDT_load:
  push ebp
  mov ebp, esp

  ; Load the GDT
  mov eax, [ebp + 8]        ; descriptor
  lgdt [eax]                ; Load the GDT

  ; Load the code segment
  mov eax, [ebp + 12]       ; codeSegment
  push eax                  ; Push the code segment
  push .reload_cs           ; Push the reload code segment label
  retf                      ; Return to the reload code segment label

.reload_cs:
  ; Load the data segment
  mov ax, [ebp + 16]        ; dataSegment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax,
  mov ss, ax

  jmp .done

.done:
  ; Restore call frame
  mov esp, ebp
  pop ebp
  ret
