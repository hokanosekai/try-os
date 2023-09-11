[bits 32]

; void __attribute__((cdecl)) IDT_load(IDT_Descriptor* descriptor)
global IDT_load
IDT_load:
  push ebp
  mov ebp, esp

  ; load IDT
  mov eax, [ebp+8]    ; descriptor
  lidt [eax]          ; load IDT

  ; restore call frame
  mov esp, ebp
  pop ebp
  ret