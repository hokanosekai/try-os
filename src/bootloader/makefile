BUILD_DIR ?= bin
ASM = nasm

.PHONY: all bootloader clean

all: bootloader

bootloader: $(BUILD_DIR)/bootloader.bin

$(BUILD_DIR)/bootloader.bin:
	$(ASM) -f bin -o $(BUILD_DIR)/bootloader.bin boot.asm

clean:
	rm -f $(BUILD_DIR)/bootloader.bin
