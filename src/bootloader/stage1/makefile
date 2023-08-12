BIN_DIR ?= bin
ASM = nasm

.PHONY: all stage1 clean

all: stage1

stage1: $(BIN_DIR)/stage1.bin

$(BIN_DIR)/stage1.bin:
	$(ASM) -f bin -o $(BIN_DIR)/stage1.bin boot.asm

clean:
	rm -f $(BIN_DIR)/stage1.bin
