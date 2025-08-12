#include <klib/types.h>
#include <klib/memory.h>
#include <mem/frames.h>
#include <mem/paging.h>
#include <bootloader/limine.h>
#include <dbg/log.h>
#include <drivers/pci.h>
#include <drivers/io.h>
#include <klib/spinlock.h>
#include <sys/timer.h>

#include <uacpi/uacpi.h>
#include <cpu/irq.h>

extern "C" {

struct uacpi_irq_handler {
  uacpi_interrupt_handler handler;
  uacpi_handle ctx;
  u32 irq;
  bool installed;
};

static uacpi_irq_handler uacpi_handlers[256] = {0};


uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address) {
  *out_rsdp_address = (u64)boot_ctx.rsdp_addr;
  return UACPI_STATUS_OK;
}

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len) {
  u64 offset = addr - ROUND_DOWN(addr);
  u64 total_len = ALIGN_UP(len + offset, 0x1000);
  void *virt = PHYS2VIRT(addr);
  Mem::Paging::map_range((void*)addr, virt, total_len, Mem::Paging::PAGE_PRESENT | Mem::Paging::PAGE_WRITE);
  return virt;
}

void uacpi_kernel_unmap(void *, uacpi_size) {
  while (0);
}

void uacpi_kernel_log(uacpi_log_level, const uacpi_char* msg) {
  Log::print(msg);
}

uacpi_status uacpi_kernel_pci_device_open(
    uacpi_pci_address address, uacpi_handle *out_handle
) {
  uacpi_pci_address* pci = (uacpi_pci_address*)kmalloc(sizeof(uacpi_pci_address));
  *pci = address;
  *out_handle = (uacpi_handle)pci;
  return UACPI_STATUS_OK;
}

void uacpi_kernel_pci_device_close(uacpi_handle hnd) {
  kfree((void*)hnd);
}

uacpi_status uacpi_kernel_pci_read8(
  uacpi_handle device, uacpi_size offset, uacpi_u8 *value
) {
  uacpi_pci_address* pci = (uacpi_pci_address*)device;
  *value = Drivers::PCI::Config::read8(pci->bus,pci->device,pci->function,offset);
  return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_pci_read16(
  uacpi_handle device, uacpi_size offset, uacpi_u16 *value
) {
  uacpi_pci_address* pci = (uacpi_pci_address*)device;
  *value = Drivers::PCI::Config::read16(pci->bus,pci->device,pci->function,offset);
  return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_read32(
  uacpi_handle device, uacpi_size offset, uacpi_u32 *value
) {
  uacpi_pci_address* pci = (uacpi_pci_address*)device;
  *value = Drivers::PCI::Config::read32(pci->bus,pci->device,pci->function,offset);
  return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_write8(
    uacpi_handle device, uacpi_size offset, uacpi_u8 value
) {
  uacpi_pci_address* pci = (uacpi_pci_address*)device;
  Drivers::PCI::Config::write8(pci->bus,pci->device,pci->function,offset,value);
  return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_pci_write16(
    uacpi_handle device, uacpi_size offset, uacpi_u16 value
) {
  uacpi_pci_address* pci = (uacpi_pci_address*)device;
  Drivers::PCI::Config::write16(pci->bus,pci->device,pci->function,offset,value);
  return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_pci_write32(
    uacpi_handle device, uacpi_size offset, uacpi_u32 value
) {
  uacpi_pci_address* pci = (uacpi_pci_address*)device;
  Drivers::PCI::Config::write32(pci->bus,pci->device,pci->function,offset,value);
  return UACPI_STATUS_OK;
}

struct uacpi_io {
    uint64_t base;
    uint64_t len;
} __attribute__((packed));

uacpi_status uacpi_kernel_io_map(
    uacpi_io_addr base, uacpi_size len, uacpi_handle *out_handle
) {
  uacpi_io* iomap = (uacpi_io*)kmalloc(sizeof(uacpi_io));
  iomap->base = base;
  iomap->len = len;
  *out_handle = (uacpi_handle*)iomap;
  return UACPI_STATUS_OK;
}
void uacpi_kernel_io_unmap(uacpi_handle handle) {
  kfree((void*)handle);
}

uacpi_status uacpi_kernel_io_read8(
    uacpi_handle hnd, uacpi_size offset, uacpi_u8 *out_value
) {
  *out_value = Drivers::IO::inb(*(u16*)hnd + offset);
  return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_read16(
    uacpi_handle hnd, uacpi_size offset, uacpi_u16 *out_value
) {
  *out_value = Drivers::IO::inw(*(u16*)hnd + offset);
  return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_read32(
    uacpi_handle hnd, uacpi_size offset, uacpi_u32 *out_value
) {
  *out_value = Drivers::IO::ind(*(u16*)hnd + offset);
  return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write8(
    uacpi_handle hnd, uacpi_size offset, uacpi_u8 in_value
) {
  Drivers::IO::outb(*(u16*)hnd + offset,in_value);
  return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write16(
    uacpi_handle hnd, uacpi_size offset, uacpi_u16 in_value
) {
  Drivers::IO::outw(*(u16*)hnd + offset,in_value);
  return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write32(
    uacpi_handle hnd, uacpi_size offset, uacpi_u32 in_value
) {
  Drivers::IO::outd(*(u16*)hnd + offset,in_value);
  return UACPI_STATUS_OK;
}

void *uacpi_kernel_alloc(uacpi_size size) {
  void* addr = kmalloc(size);
  return addr;
}

void uacpi_kernel_free(void *mem) {
  kfree(mem);
}

uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void) {
  return Sys::Timer::get_uptime_ns();
}

void uacpi_kernel_stall(uacpi_u8 usec) {
  Sys::Timer::sleep_us(usec);
}

void uacpi_kernel_sleep(uacpi_u64 msec) {
  Sys::Timer::sleep_ms(msec);
}

uacpi_handle uacpi_kernel_create_mutex(void) {
  return (uacpi_handle) new Spinlock;
}

void uacpi_kernel_free_mutex(uacpi_handle hnd) {
  kfree((void*)hnd);
}

uacpi_handle uacpi_kernel_create_event(void) {
  return (uacpi_handle)1;
}

void uacpi_kernel_free_event(uacpi_handle) {
  while (0);
}

uacpi_thread_id uacpi_kernel_get_thread_id(void) {
  return 0;
}

uacpi_status uacpi_kernel_acquire_mutex(uacpi_handle lock, uacpi_u16) {
  Spinlock* slock = (Spinlock*)lock;
  slock->lock();
  return UACPI_STATUS_OK;
}

void uacpi_kernel_release_mutex(uacpi_handle lock) {
  Spinlock* slock = (Spinlock*)lock;
  slock->unlock();
}

uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle, uacpi_u16) {
  return 1;
}

void uacpi_kernel_signal_event(uacpi_handle) {
  __asm__ __volatile__("nop");
}

void uacpi_kernel_reset_event(uacpi_handle) {
  __asm__ __volatile__("nop");
}

uacpi_status uacpi_kernel_handle_firmware_request(uacpi_firmware_request*) {
  return UACPI_STATUS_OK;
}

void uacpi_irq_bridge(struct irq_frame *frame) {
  u32 irq = frame->error;
  
  if (irq >= 256) return;
  
  struct uacpi_irq_handler *handler = &uacpi_handlers[irq];
  if (!handler->installed || !handler->handler) return;
  
  handler->handler(handler->ctx);
}

uacpi_status uacpi_kernel_install_interrupt_handler(
    uacpi_u32 irq, uacpi_interrupt_handler handler, uacpi_handle ctx,
    uacpi_handle *out_irq_handle
) {
  if (irq >= 256 || !handler || !out_irq_handle) {
    return UACPI_STATUS_INVALID_ARGUMENT;
  }
  
  if (uacpi_handlers[irq].installed) {
    return UACPI_STATUS_ALREADY_EXISTS;
  }
  
  uacpi_handlers[irq].handler = handler;
  uacpi_handlers[irq].ctx = ctx;
  uacpi_handlers[irq].irq = irq;
  uacpi_handlers[irq].installed = true;
  
  IRQ::register_entry((u8)irq, uacpi_irq_bridge);
  
  *out_irq_handle = (uacpi_handle)&uacpi_handlers[irq];
  
  return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_uninstall_interrupt_handler(
    uacpi_interrupt_handler handler, uacpi_handle irq_handle
) {
  if (!handler || !irq_handle) {
    return UACPI_STATUS_INVALID_ARGUMENT;
  }
  
  struct uacpi_irq_handler *uacpi_handler = (struct uacpi_irq_handler*)irq_handle;
  
  if (uacpi_handler < &uacpi_handlers[0] || 
      uacpi_handler >= &uacpi_handlers[256] ||
      !uacpi_handler->installed ||
      uacpi_handler->handler != handler) {
    return UACPI_STATUS_INVALID_ARGUMENT;
  }
  
  uacpi_handler->installed = false;
  uacpi_handler->handler = nullptr;
  uacpi_handler->ctx = nullptr;
  
  return UACPI_STATUS_OK;
}

uacpi_handle uacpi_kernel_create_spinlock(void) {
  return (uacpi_handle) new Spinlock;
}

void uacpi_kernel_free_spinlock(uacpi_handle hnd) {
 kfree((void*)hnd);
}

uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle hnd) {
  Spinlock* lock = (Spinlock*)hnd;
  lock->lock();
  return UACPI_STATUS_OK;
}

void uacpi_kernel_unlock_spinlock(uacpi_handle hnd, uacpi_cpu_flags) {
  Spinlock* lock = (Spinlock*)hnd;
  lock->unlock();
}

uacpi_status uacpi_kernel_schedule_work(
    uacpi_work_type, uacpi_work_handler, uacpi_handle
) {
  return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_wait_for_work_completion(void) {
  return UACPI_STATUS_OK;
}


}
