include config/config.mk

.PHONY: all floppy kernel bootloader clean always

all: floppy

include config/toolchain.mk

# Floppy
floppy: $(BUILD_DIR)/floppy.img

$(BUILD_DIR)/floppy.img: bootloader kernel
	dd if=/dev/zero of=$(BUILD_DIR)/floppy.img bs=512 count=2880 > /dev/null
	mkfs.fat -F 12 -n "TRYOS" $(BUILD_DIR)/floppy.img > /dev/null
	dd if=$(BUILD_DIR)/bootloader.bin of=$(BUILD_DIR)/floppy.img conv=notrunc > /dev/null
	mcopy -i $@ $(BUILD_DIR)/kernel.bin "::kernel.bin"
	mcopy -i $@ test.txt "::test.txt"
	echo "Floppy: Done"

# Bootloader
bootloader: $(BUILD_DIR)/bootloader.bin

$(BUILD_DIR)/bootloader.bin: always
	$(MAKE) -C src/bootloader BUILD_DIR=$(abspath $(BUILD_DIR))

# Kernel
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	$(MAKE) -C src/kernel BUILD_DIR=$(abspath $(BUILD_DIR))

# Always
always:
	mkdir -p $(BUILD_DIR)

# Clean
clean:
	$(MAKE) -C src/bootloader clean BUILD_DIR=$(abspath $(BUILD_DIR))
	$(MAKE) -C src/kernel clean BUILD_DIR=$(abspath $(BUILD_DIR))
	rm -rf $(BUILD_DIR)