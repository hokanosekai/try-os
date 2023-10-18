BUILD_DIR ?= bin
ASM = nasm

.PHONY: all stage1 clean

all: stage1

stage1: $(BUILD_DIR)/stage1.bin

$(BUILD_DIR)/stage1.bin:
	$(ASM) -f bin -o $(BUILD_DIR)/stage1.bin boot.asm

clean:
	rm -f $(BUILD_DIR)/stage1.bin
