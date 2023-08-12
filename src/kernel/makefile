BIN_DIR ?= bin
ASM = nasm

.PHONY: all kernel clean

all: kernel

kernel: $(BIN_DIR)/kernel.bin

$(BIN_DIR)/kernel.bin:
	$(ASM) -f bin -o $(BIN_DIR)/kernel.bin main.asm

clean:
	rm -f $(BIN_DIR)/kernel.bin