VERSION := $(shell git rev-parse --short HEAD)

LIMINE_DIR := misc/limine
LIMINE_BIN := $(LIMINE_DIR)/limine
LIMINE_CFG := misc/boot/limine.conf


INCLUDES := -I$(LIMINE_DIR) -Isrc/include
KERNEL_SRC := src/kernel
LINKER_SCRIPT = misc/linkage.ld
KERNEL_BIN := build/kernel.bin

OBJ_DIR := build/obj

LIBC_SRC := src/libc
LIBC_BIN := $(OBJ_DIR)/libc.o

ISO_FILE := build/slayer_$(VERSION).iso
ISO_DIR := build/iso

include misc/make/base.mk

KERN_SOURCES := $(shell find $(KERNEL_SRC) -name '*.c' -or -name '*.s')
KERN_OBJECTS := $(patsubst $(KERNEL_SRC)/%.c, $(OBJ_DIR)/%.o, $(patsubst $(KERNEL_SRC)/%.s, $(OBJ_DIR)/%.o, $(KERN_SOURCES)))

LIBC_SOURCES := $(shell find $(LIBC_SRC) -name '*.c' -or -name '*.s')
LIBC_OBJECTS := $(patsubst $(LIBC_SRC)/%.c, $(OBJ_DIR)/%.o, $(patsubst $(LIBC_SRC)/%.s, $(OBJ_DIR)/%.o, $(LIBC_SOURCES)))

override CFLAGS += -DSLAY_VERSION=\"$(VERSION)\"

all: $(ISO_FILE)

# LIBC

$(LIBC_BIN): $(LIBC_OBJECTS)
	$(LD) -r $(LIBC_+LDFLAGS) -o $(LIBC_BIN) $(LIBC_OBJECTS)

$(OBJ_DIR)/%.o: $(LIBC_SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIBC_SRC)/%.s
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Kernel

$(OBJ_DIR)/%.o: $(KERNEL_SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(KERNEL_SRC)/%.s
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(LIBC_BIN) $(KERN_OBJECTS)
	$(LD) $(KERN_LDFLAGS) -o $(KERNEL_BIN) $(KERN_OBJECTS) $(LIBC_BIN)


# Limine

$(LIMINE_BIN):
	bash misc/scripts/limine_bootstrap.sh

$(ISO_FILE): $(LIMINE_BIN) $(KERNEL_BIN)
	mkdir -p $(ISO_DIR)/boot/limine
	mkdir -p $(ISO_DIR)/EFI/BOOT

	cp $(KERNEL_BIN) $(ISO_DIR)/boot/slay.kernel
	cp -v $(LIMINE_CFG) $(LIMINE_DIR)/limine-bios.sys $(LIMINE_DIR)/limine-bios-cd.bin $(LIMINE_DIR)/limine-uefi-cd.bin $(ISO_DIR)/boot/limine
	cp -v $(LIMINE_DIR)/BOOTX64.EFI $(LIMINE_DIR)/BOOTIA32.EFI $(ISO_DIR)/EFI/BOOT
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
		-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		$(ISO_DIR) -o $@
	$(LIMINE_BIN) bios-install $@


# Testing

run: $(ISO_FILE)
	qemu-system-x86_64 $(QEMU_ARGS) -cdrom $<


# Misc

clean:
	rm -rf build
