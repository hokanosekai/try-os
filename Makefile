ASM = nasm

SRC_DIR = src
BIN_DIR = bin

$(BIN_DIR)/main_floppy.img: $(BIN_DIR)/main.bin
	cp $(BIN_DIR)/main.bin $(BIN_DIR)/floppy.img
	truncate -s 1440k $(BIN_DIR)/floppy.img

$(BIN_DIR)/main.bin: $(SRC_DIR)/main.asm
	$(ASM) -f bin -o $(BIN_DIR)/main.bin $(SRC_DIR)/main.asm