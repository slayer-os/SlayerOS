VERSION := $(shell git rev-parse --short HEAD)

LIMINE_DIR := limine
LIMINE_BIN := $(LIMINE_DIR)/limine
LIMINE_CFG := misc/boot/limine.conf

KLIB_DIR := klib
DRIVERS_DIR := drivers
ZYDIS_DIR := zydis

INCLUDES := -I$(shell realpath $(LIMINE_DIR)) \
						-I$(KLIB_DIR)/src/include \
						-I$(DRIVERS_DIR)/src/include \
						-I$(ZYDIS_DIR)/include \
						-I$(ZYDIS_DIR)/dependencies/zycore/include \
						-Isrc/include

KERNEL_SRC := src/kernel
LINKER_SCRIPT = misc/linkage.ld
KERNEL_BIN := build/kernel.bin

OBJ_DIR := build/obj

KLIB_LIB := $(KLIB_DIR)/build/klib.a
DRIVERS_LIB := $(DRIVERS_DIR)/build/drivers.a
ZYDIS_LIB := $(ZYDIS_DIR)/build/libZydis.a

ISO_FILE := build/slayer_$(VERSION).iso
ISO_DIR := build/iso


include misc/make/base.mk
override CFLAGS += -DSLAY_VERSION=\"$(VERSION)\"

KERN_SOURCES := $(shell find $(KERNEL_SRC) -name '*.cc' -or -name '*.s')
KERN_OBJECTS := $(patsubst $(KERNEL_SRC)/%.cc, $(OBJ_DIR)/%.o, $(patsubst $(KERNEL_SRC)/%.s, $(OBJ_DIR)/%.o, $(KERN_SOURCES)))

LIMINE_MAKEFILE := $(LIMINE_DIR)/Makefile
KLIB_MAKEFILE := $(KLIB_DIR)/Makefile
DRIVERS_MAKEFILE := $(DRIVERS_DIR)/Makefile


all: $(ISO_FILE)

$(LIMINE_MAKEFILE):
	git submodule update --init --recursive

$(KLIB_MAKEFILE):
	git submodule update --init --recursive

$(DRIVERS_MAKEFILE):
	git submodule update --init --recursive

# KLIB

$(KLIB_LIB): $(KLIB_MAKEFILE)
	$(MAKE) -C $(KLIB_DIR)

# Drivers

$(DRIVERS_LIB): $(DRIVERS_MAKEFILE)
	$(MAKE) -C $(DRIVERS_DIR)

# Zydis

$(ZYDIS_LIB):
	cmake -B $(ZYDIS_DIR)/build -S $(ZYDIS_DIR) -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-fno-stack-protector"
	cmake --build $(ZYDIS_DIR)/build -j$(nproc) --target Zydis

# Limine

$(LIMINE_BIN): $(LIMINE_MAKEFILE)
	$(MAKE) -C $(LIMINE_DIR)

# Kernel

$(OBJ_DIR)/%.o: $(KERNEL_SRC)/%.cc
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(KERNEL_SRC)/%.s
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(ZYDIS_LIB) $(KLIB_LIB) $(DRIVERS_LIB) $(KERN_OBJECTS)
	$(LD) $(KERN_LDFLAGS) -o $(KERNEL_BIN) $(KERN_OBJECTS) $(DRIVERS_LIB) $(KLIB_LIB) $(ZYDIS_LIB)


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
	qemu-system-x86_64 $(QEMU_ARGS) $(QEMU_OPT) -cdrom $<

runint: $(ISO_FILE)
	qemu-system-x86_64 $(QEMU_ARGS) $(QEMU_OPT) -cdrom $< -d int

debug: $(ISO_FILE)
	qemu-system-x86_64 $(QEMU_ARGS) $(QEMU_OPT) -cdrom $< -s -S

gdb:
	sudo gdb -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL_BIN)" -ex "hbreak _start" -ex "c" -ex "layout src" -ex "layout split"

remote:
	qemu-system-x86_64 $(QEMU_ARGS) -display none -cdrom $(ISO_FILE) -serial stdio -monitor telnet:0.0.0.0:4444,server,wait

# Misc

clean:
	rm -rf build
	$(MAKE) -C $(KLIB_DIR) clean
	$(MAKE) -C $(DRIVERS_DIR) clean
	rm -r $(ZYDIS_DIR)/build

.PHONY: all clean run runint $(KLIB_LIB) $(DRIVERS_LIB)
