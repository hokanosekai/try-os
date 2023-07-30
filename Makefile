ASM = nasm

SRC_DIR = src
BIN_DIR = bin

.PHONY: all floppy kernel bootloader clean always

# Floppy
floppy: $(BIN_DIR)/floppy.img

$(BIN_DIR)/floppy.img: bootloader kernel
	dd if=/dev/zero of=$(BIN_DIR)/floppy.img bs=512 count=2880
	mkfs.fat -F 12 -n "TRYOS" $(BIN_DIR)/floppy.img
	dd if=$(BIN_DIR)/bootloader.bin of=$(BIN_DIR)/floppy.img conv=notrunc
	mcopy -i $(BIN_DIR)/floppy.img $(BIN_DIR)/kernel.bin "::kernel.bin"

# Bootloader
bootloader: $(BIN_DIR)/bootloader.bin

$(BIN_DIR)/bootloader.bin: always
	$(ASM) -f bin -o $(BIN_DIR)/bootloader.bin $(SRC_DIR)/bootloader/boot.asm

# Kernel
kernel: $(BIN_DIR)/kernel.bin

$(BIN_DIR)/kernel.bin: always
	$(ASM) -f bin -o $(BIN_DIR)/kernel.bin $(SRC_DIR)/kernel/main.asm

# Always
always:
	mkdir -p $(BIN_DIR)

# Clean
clean:
	rm -rf $(BIN_DIR)