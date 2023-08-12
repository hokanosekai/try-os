ASM = nasm
CC = gcc
CC16 = wcc
LD16 = wlink
MAKE = make

SRC_DIR = src
BIN_DIR = bin

.PHONY: all floppy kernel bootloader clean always

# Floppy
floppy: $(BIN_DIR)/floppy.img

$(BIN_DIR)/floppy.img: bootloader kernel
	dd if=/dev/zero of=$(BIN_DIR)/floppy.img bs=512 count=2880
	mkfs.fat -F 12 -n "TRYOS" $(BIN_DIR)/floppy.img
	dd if=$(BIN_DIR)/stage1.bin of=$(BIN_DIR)/floppy.img conv=notrunc
	mcopy -i $(BIN_DIR)/floppy.img $(BIN_DIR)/stage2.bin "::stage2.bin"
	mcopy -i $(BIN_DIR)/floppy.img $(BIN_DIR)/kernel.bin "::kernel.bin"

# Bootloader
bootloader: stage1 stage2

stage1: $(BIN_DIR)/stage1.bin

$(BIN_DIR)/stage1.bin: always
	$(MAKE) -C $(SRC_DIR)/bootloader/stage1 BIN_DIR=$(abspath $(BIN_DIR))

stage2: $(BIN_DIR)/stage2.bin

$(BIN_DIR)/stage2.bin: always
	$(MAKE) -C $(SRC_DIR)/bootloader/stage2 BIN_DIR=$(abspath $(BIN_DIR))

# Kernel
kernel: $(BIN_DIR)/kernel.bin

$(BIN_DIR)/kernel.bin: always
	$(MAKE) -C $(SRC_DIR)/kernel BIN_DIR=$(abspath $(BIN_DIR))

# Always
always:
	mkdir -p $(BIN_DIR)

# Clean
clean:
	$(MAKE) -C $(SRC_DIR)/bootloader/stage1 clean BIN_DIR=$(abspath $(BIN_DIR))
	$(MAKE) -C $(SRC_DIR)/bootloader/stage2 clean BIN_DIR=$(abspath $(BIN_DIR))
	$(MAKE) -C $(SRC_DIR)/kernel clean BIN_DIR=$(abspath $(BIN_DIR))
	rm -rf $(BIN_DIR)