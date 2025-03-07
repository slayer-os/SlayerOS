
# -g is essential as the kernel tries to print a stack trace on panic
# and it needs debug info to lookup the function source file and line number

override CFLAGS := -g -fno-inline-small-functions \
	-Wall \
	-Wextra \
	-std=c++17 \
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
	-Wno-missing-field-initializers \
	-Wno-pointer-arith \
	-O3

override LIBC_LDFLAGS := -nostdlib -z max-page-size=0x1000
override KERN_LDFLAGS += -m elf_x86_64 \
	-nostdlib \
	-pie \
	-z text \
	-z max-page-size=0x1000 \
	-T $(LINKER_SCRIPT)

QEMU_CPU_MODEL := host
QEMU_CPUID :=\
						+xsave,+ssse3,+sse4.1,+sse4.2,+x2apic,+avx,enforce

QEMU_ARGS := \
						 --enable-kvm -cpu $(QEMU_CPU_MODEL),$(QEMU_CPUID) -smp 2,maxcpus=8 \
						 --no-reboot

QEMU_OPT := -serial stdio -vga virtio -display sdl --no-shutdown
