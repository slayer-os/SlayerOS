
# SlayerOS  

SlayerOS is a lightweight, fast, and sleek operating system designed for **bare-metal x86_64 processors**. It features a **RAM-based execution model**, a **basic shell**, and a **simple filesystem** for file storage.  

## **Features**  
✅ **Bootable Kernel** (Multiboot2-compatible)  
✅ **RAM-based Filesystem**  
✅ **Arch Linux-style CLI** (`cd`, `ls`, `echo`)  
✅ **Serial Debugging Output**  
✅ **Frutiger Aero Boot Splash**  
✅ **QEMU & ISO Support**  

---

## **Building SlayerOS**  

### **1. Install Dependencies (Arch Linux)**  
First, install the required packages:  
```sh
sudo pacman -S --needed base-devel qemu nasm xorriso
```
Then, install the cross-compiler toolchain:  
```sh
yay -S x86_64-elf-gcc x86_64-elf-binutils x86_64-elf-newlib
```

---

### **2. Compile the Kernel**  
Use the following commands to compile SlayerOS:  
```sh
nasm -f elf64 start.s -o start.o
x86_64-elf-gcc -ffreestanding -mno-red-zone -c kernel.c -o kernel.o
x86_64-elf-gcc -ffreestanding -mno-red-zone -c serial.c -o serial.o
x86_64-elf-gcc -ffreestanding -mno-red-zone -c shell.c -o shell.o
x86_64-elf-gcc -ffreestanding -mno-red-zone -c fs.c -o fs.o
x86_64-elf-gcc -ffreestanding -mno-red-zone -c memory.c -o memory.o
x86_64-elf-ld -n -T linker.ld -o kernel.elf start.o kernel.o serial.o shell.o fs.o memory.o
```

---

### **3. Run SlayerOS in QEMU**  
To test SlayerOS in a virtual machine, use:  
```sh
qemu-system-x86_64 -kernel kernel.elf -serial stdio
```
This boots the OS and prints serial output to the terminal.

---

## **Creating a Bootable ISO**  
If you want to boot SlayerOS from an **ISO image**, follow these steps:

### **1. Install GRUB for Multiboot**  
```sh
sudo pacman -S grub
```

### **2. Create a GRUB Configuration (`grub.cfg`)**  
```sh
mkdir -p iso/boot/grub
cat > iso/boot/grub/grub.cfg << EOF
menuentry "SlayerOS" {
    multiboot2 /boot/kernel.elf
    boot
}
EOF
```

### **3. Copy Kernel to Boot Directory**  
```sh
mkdir -p iso/boot
cp kernel.elf iso/boot/
```

### **4. Generate ISO**  
```sh
grub-mkrescue -o SlayerOS.iso iso
```

### **5. Run ISO in QEMU**  
```sh
qemu-system-x86_64 -cdrom SlayerOS.iso
```

---

## **Shell Commands**  
The SlayerOS shell currently supports the following commands:  
- `help` → Display available commands  
- `ls` → List files in RAM-based filesystem  
- `cd` → Change directories (planned)  
- `echo` → Print text to the screen  
- `clear` → Clear the terminal  

---

## **Project Goals**  
- Implement a **custom GPU driver** to replace framebuffer reliance  
- Expand the **RAM-based filesystem** with file read/write functionality  
- Improve **CLI interactivity** with keyboard input  
- Add **basic process scheduling**  

---

## **License**  
SlayerOS is an experimental project so far <3
```

---

### **Summary of the `README.md`**
- **Overview** of SlayerOS  
- **Installation & Build Instructions**  
- **How to Run SlayerOS in QEMU**  
- **Creating a Bootable ISO**  
- **List of Available Shell Commands**  
- **Future Development Goals**  

---
