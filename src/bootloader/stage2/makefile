BIN_DIR ?= bin
ASM = nasm
CC16 = wcc
LD16 = wlink

CFLAGS16 = -4 -d3 -s -wx -ms -zl -zq
ASMFLAGS = -f obj

SOURCES_ASM = $(wildcard *.asm)
SOURCES_C = $(wildcard *.c)

OBJECTS_ASM = $(patsubst %.asm, $(BIN_DIR)/stage2/asm/%.o, $(SOURCES_ASM))
OBJECTS_C = $(patsubst %.c, $(BIN_DIR)/stage2/c/%.o, $(SOURCES_C))

.PHONY: all stage2 clean always

all: stage2

stage2: $(BIN_DIR)/stage2.bin

$(BIN_DIR)/stage2.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	$(LD16) name $(BIN_DIR)/stage2.bin file \{ $(OBJECTS_ASM) $(OBJECTS_C) \} option map=$(BIN_DIR)/stage2.map @linker.lnk

$(BIN_DIR)/stage2/c/%.o: %.c always
	$(CC16) $(CFLAGS16) -fo=$@ $<

$(BIN_DIR)/stage2/asm/%.o: %.asm always
	$(ASM) $(ASMFLAGS) -o $@ $<

always:
	mkdir -p $(BIN_DIR)/stage2/c
	mkdir -p $(BIN_DIR)/stage2/asm

clean:
	rm -f $(BIN_DIR)/stage2.bin