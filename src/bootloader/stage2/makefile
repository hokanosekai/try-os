TARGET_ASMFLAGS += -f elf
TARGET_CFLAGS += -ffreestanding\
 -nostdlib\
 -I./include
TARGET_LIBS += -lgcc
TARGET_LINKFLAGS += -T linker.ld -nostdlib

SOURCES_ASM = $(wildcard *.asm) $(wildcard */*.asm)
SOURCES_C = $(wildcard *.c) $(wildcard */*.c)

OBJECTS_ASM = $(patsubst %.asm, $(BUILD_DIR)/stage2/asm/%.o, $(SOURCES_ASM))
OBJECTS_C = $(patsubst %.c, $(BUILD_DIR)/stage2/c/%.o, $(SOURCES_C))

.PHONY: all stage2 clean always

all: stage2

stage2: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/stage2.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	@$(TARGET_LD) $(TARGET_LINKFLAGS) -Wl,-Map=$(BUILD_DIR)/stage2.map -o $@ $^ $(TARGET_LIBS)
	@echo "stage2: Done"

$(BUILD_DIR)/stage2/c/%.o: %.c
	@mkdir -p $(@D)
	@$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<
	@echo $< ": Done"

$(BUILD_DIR)/stage2/asm/%.o: %.asm
	@mkdir -p $(@D)
	@$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<
	@echo $< ": Done"

clean:
	rm -f $(BUILD_DIR)/stage2.bin