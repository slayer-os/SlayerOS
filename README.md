
# SlayerOS  

SlayerOS is a lightweight, fast, and sleek operating system designed for **bare-metal x86_64 processors**.

## **Features**  
✅ **It might boot if you ask NICELY**  

---

## **Building SlayerOS**  

### **1. Install Dependencies (Arch Linux)**  
First, install the required packages:  
```sh
sudo pacman -S --needed base-devel qemu make xorriso
```

---

### **2. Build the Kernel**  
Use the following commands to compile SlayerOS and package it into an ISO:
```sh
make
```
the ISO will be created in the build/ directory

---

### **3. Run SlayerOS in QEMU**  
To test SlayerOS in qemu, use:
```sh
make run
```

---

## **Project Goals**  
- Implement a **custom GPU driver** to replace framebuffer reliance  
- Expand the **RAM-based filesystem** with file read/write functionality  
- Add **basic process scheduling**  

---

## **License**  
SlayerOS is an experimental project and is licensed under the CCLV1 (Chiyo's Creative License Version 1) with love <3
