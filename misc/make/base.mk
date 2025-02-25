
override CFLAGS := -Wall \
	-Wextra \
	-std=gnu11 \
	-ffreestanding \
	-fno-stack-protector \
	-fno-stack-check \
	-fno-lto \
	-m64 \
	-march=x86-64 \
	-mno-80387 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-O3

override LIBC_LDFLAGS := -nostdlib -z max-page-size=0x1000
override KERN_LDFLAGS += -m elf_x86_64 \
	-nostdlib \
	-pie \
	-z text \
	-z max-page-size=0x1000 \
	-T $(LINKER_SCRIPT)


QEMU_ARGS := -smp 2 -serial stdio --no-reboot --no-shutdown -vga virtio -display sdl
