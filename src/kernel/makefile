TARGET_ASMFLAGS += -f elf
TARGET_CFLAGS += -ffreestanding\
 -nostdlib\
 -I./include
TARGET_LIBS += -lgcc
TARGET_LINKFLAGS += -T linker.ld -nostdlib

SOURCES_ASM = $(wildcard *.asm) $(wildcard */*.asm)
SOURCES_C = $(wildcard *.c) $(wildcard */*.c)

OBJECTS_ASM = $(patsubst %.asm, $(BUILD_DIR)/kernel/asm/%.o, $(SOURCES_ASM))
OBJECTS_C = $(patsubst %.c, $(BUILD_DIR)/kernel/c/%.o, $(SOURCES_C))

.PHONY: all kernel clean always

all: kernel

kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	@$(TARGET_LD) $(TARGET_LINKFLAGS) -Wl,-Map=$(BUILD_DIR)/kernel.map -o $@ $^ $(TARGET_LIBS)
	@echo "Kernel: Done"

$(BUILD_DIR)/kernel/c/%.o: %.c
	@mkdir -p $(@D)
	@$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<
	@echo $< ": Done"

$(BUILD_DIR)/kernel/asm/%.o: %.asm
	@mkdir -p $(@D)
	@$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<
	@echo $< ": Done"

clean:
	rm -f $(BUILD_DIR)/kernel.bin