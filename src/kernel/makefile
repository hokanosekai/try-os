BUILD_DIR ?= bin
ASM = nasm

.PHONY: all kernel clean

all: kernel

kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin:
	$(ASM) -f bin -o $(BUILD_DIR)/kernel.bin main.asm

clean:
	rm -f $(BUILD_DIR)/kernel.bin