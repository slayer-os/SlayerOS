VERSION := $(shell git rev-parse --short HEAD)

LIMINE_DIR := limine
LIMINE_BIN := $(LIMINE_DIR)/limine
LIMINE_CFG := misc/boot/limine.conf

LIBC_DIR := libc
DRIVERS_DIR := drivers

INCLUDES := -I$(shell realpath $(LIMINE_DIR)) \
						-I$(LIBC_DIR)/src/include \
						-I$(DRIVERS_DIR)/src/include \
						-Isrc/include

KERNEL_SRC := src/kernel
LINKER_SCRIPT = misc/linkage.ld
KERNEL_BIN := build/kernel.bin

OBJ_DIR := build/obj

LIBC_LIB := $(LIBC_DIR)/build/libc.a
DRIVERS_LIB := $(DRIVERS_DIR)/build/drivers.a

ISO_FILE := build/slayer_$(VERSION).iso
ISO_DIR := build/iso


include misc/make/base.mk
override CFLAGS += -DSLAY_VERSION=\"$(VERSION)\"

KERN_SOURCES := $(shell find $(KERNEL_SRC) -name '*.c' -or -name '*.s')
KERN_OBJECTS := $(patsubst $(KERNEL_SRC)/%.c, $(OBJ_DIR)/%.o, $(patsubst $(KERNEL_SRC)/%.s, $(OBJ_DIR)/%.o, $(KERN_SOURCES)))

LIMINE_MAKEFILE := $(LIMINE_DIR)/Makefile
LIBC_MAKEFILE := $(LIBC_DIR)/Makefile
DRIVERS_MAKEFILE := $(DRIVERS_DIR)/Makefile


all: $(ISO_FILE)

$(LIMINE_MAKEFILE):
	git submodule update --init --recursive

$(LIBC_MAKEFILE):
	git submodule update --init --recursive

$(DRIVERS_MAKEFILE):
	git submodule update --init --recursive

# LIBC

$(LIBC_LIB): $(LIBC_MAKEFILE)
	$(MAKE) -C $(LIBC_DIR)

# Drivers

$(DRIVERS_LIB): $(DRIVERS_MAKEFILE)
	$(MAKE) -C $(DRIVERS_DIR)

# Limine

$(LIMINE_BIN): $(LIMINE_MAKEFILE)
	$(MAKE) -C $(LIMINE_DIR)

# Kernel

$(OBJ_DIR)/%.o: $(KERNEL_SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(KERNEL_SRC)/%.s
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(LIBC_LIB) $(DRIVERS_LIB) $(KERN_OBJECTS)
	$(LD) $(KERN_LDFLAGS) -o $(KERNEL_BIN) $(KERN_OBJECTS) $(DRIVERS_LIB) $(LIBC_LIB) 


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

runint: $(ISO_FILE)
	qemu-system-x86_64 $(QEMU_ARGS) -cdrom $< -d int

# Misc

clean:
	rm -rf build
	$(MAKE) -C $(LIBC_DIR) clean
	$(MAKE) -C $(DRIVERS_DIR) clean

.PHONY: all clean run runint
