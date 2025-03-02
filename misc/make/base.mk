
# -g is essential as the kernel tries to print a stack trace on panic
# and it needs debug info to lookup the function source file and line number

override CFLAGS := -g -fno-inline-small-functions \
	-Wall \
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
	-mno-red-zone \
	-mno-sse \
	-mno-sse2 \
	-O3

override LIBC_LDFLAGS := -nostdlib -z max-page-size=0x1000
override KERN_LDFLAGS += -m elf_x86_64 \
	-nostdlib \
	-pie \
	-z text \
	-z max-page-size=0x1000 \
	-T $(LINKER_SCRIPT)


QEMU_ARGS := \
						 -cpu qemu64,+ssse3,+sse4.1,+sse4.2,+x2apic -smp 2,maxcpus=8 \
						 -serial stdio \
						 --no-reboot \
						 --no-shutdown \
						 -vga virtio -display sdl
