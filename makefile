# Makefile for SlayerOS

# Compiler settings
CC = gcc
AS = nasm
LD = ld
QEMU = qemu-system-x86_64

# Paths
BUILD_DIR = build
SRC_DIR = src
OBJ_DIR = $(BUILD_DIR)/obj
KERNEL_DIR = $(BUILD_DIR)/kernel

# Files
KERNEL_SRC = $(SRC_DIR)/kernel.c
BOOTLOADER = limine
BOOTLOADER_DIR = $(SRC_DIR)/bootloader
CFLAGS = -ffreestanding -O2 -Wall -Wextra -nostdlib -fno-builtin -m64 -g
LDFLAGS = -ffreestanding -O2 -nostdlib -T linker.ld
ASFLAGS = -f elf64

# Targets
all: run

# Kernel build
$(KERNEL_DIR)/kernel.o: $(KERNEL_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(KERNEL_SRC) -o $(OBJ_DIR)/kernel.o

# Bootloader (Limine) setup
$(BOOTLOADER):
	@echo "Building bootloader..."
	# Assuming Limine is pre-configured
	$(MAKE) -C $(BOOTLOADER_DIR)

# Link kernel
$(KERNEL_DIR)/kernel.bin: $(KERNEL_DIR)/kernel.o $(BOOTLOADER)
	@mkdir -p $(KERNEL_DIR)
	$(LD) $(LDFLAGS) -o $(KERNEL_DIR)/kernel.bin $(OBJ_DIR)/kernel.o

# Create ISO for booting
$(BUILD_DIR)/slayeros.iso: $(KERNEL_DIR)/kernel.bin
	@echo "Creating bootable ISO..."
	# Assuming Limine config files and ISO build process
	limine/limine-install $(KERNEL_DIR)/kernel.bin $(BUILD_DIR)/slayeros.iso

# Run on QEMU
run: $(BUILD_DIR)/slayeros.iso
	$(QEMU) -cdrom $(BUILD_DIR)/slayeros.iso

clean:
	rm -rf $(BUILD_DIR)
