include config/config.mk

.PHONY: all floppy kernel bootloader clean always

all: floppy

include config/toolchain.mk

# Floppy
floppy: $(BUILD_DIR)/floppy.img

$(BUILD_DIR)/floppy.img: bootloader kernel
	dd if=/dev/zero of=$(BUILD_DIR)/floppy.img bs=512 count=2880 > /dev/null
	mkfs.fat -F 12 -n "TRYOS" $(BUILD_DIR)/floppy.img > /dev/null
	dd if=$(BUILD_DIR)/stage1.bin of=$(BUILD_DIR)/floppy.img conv=notrunc > /dev/null
	mcopy -i $@ $(BUILD_DIR)/stage2.bin "::stage2.bin"
	mcopy -i $@ $(BUILD_DIR)/kernel.bin "::kernel.bin"
	mcopy -i $@ test.txt "::test.txt"
	echo "Floppy: Done"

# Bootloader
bootloader: stage1 stage2

stage1: $(BUILD_DIR)/stage1.bin

$(BUILD_DIR)/stage1.bin: always
	$(MAKE) -C src/bootloader/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/stage2.bin: always
	$(MAKE) -C src/bootloader/stage2 BUILD_DIR=$(abspath $(BUILD_DIR))

# Kernel
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	$(MAKE) -C src/kernel BUILD_DIR=$(abspath $(BUILD_DIR))

# Always
always:
	mkdir -p $(BUILD_DIR)

# Clean
clean:
	$(MAKE) -C src/bootloader/stage1 clean BUILD_DIR=$(abspath $(BUILD_DIR))
	$(MAKE) -C src/bootloader/stage2 clean BUILD_DIR=$(abspath $(BUILD_DIR))
	$(MAKE) -C src/kernel clean BUILD_DIR=$(abspath $(BUILD_DIR))
	rm -rf $(BUILD_DIR)