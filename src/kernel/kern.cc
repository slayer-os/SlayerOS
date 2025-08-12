#include <arch/serial.h>
#include <arch/cpuid.h>
#include <dbg/log.h>

#include <host/flags.h>

#include <bootloader/limine.h>

#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/lapic.h>
#include <cpu/irq.h>
#include <cpu/pic.h>
#include <cpu/pit.h>

#include <mem/frames.h>
#include <mem/paging.h>
#include <mem/heap.h>

#include <klib/string.h>
#include <klib/assert.h>
#include <klib/elf.h>
#include <klib/intx.h>
#include <klib/dwarf.h>
#include <arch/sse.h>
#include <task/scheduler.h>
#include <cpu/tss.h>

#include <sys/acpi.h>
#include <sys/ioapic.h>
#include <sys/timer.h>

#include <err/handler.h>

#include <drivers/fb_gfx.h>
#include <drivers/tty.h>
#include <klib/spinlock.h>

void _kernel_pre_setup() {
  UART::init();
  boot_ctx.gather();
  Drivers::fb::gfx::init();
  Drivers::TTY::init(boot_ctx.fb_info->framebuffers[0]);
  
  Drivers::fb::gfx::splash::sevastolink(0);

  GDT::init();
  Drivers::fb::gfx::splash::sevastolink(5);
  Mem::Frame::init();
  Drivers::fb::gfx::splash::sevastolink(8);
  TSS::init_stacks();
  Drivers::fb::gfx::splash::sevastolink(14);
  IDT::init();
  Drivers::fb::gfx::splash::sevastolink(20);
  
  Mem::Paging::init(); Log::success("Memory mapped"); Log::success("Paging initialized");
  Drivers::fb::gfx::splash::sevastolink(25);
  Err::Handler::init_symbols(); Log::success("Kernel symbols mapped");
  Drivers::fb::gfx::splash::sevastolink(30);

  if (SSE::enable() && SSE::test()) Log::success("SSE extensions enabled");
  else Log::critical("Failed to enable SSE");

  Drivers::fb::gfx::splash::sevastolink(35);

  Sys::ACPI::init();
  Drivers::fb::gfx::splash::sevastolink(40);

  PIC::disable();
  Drivers::fb::gfx::splash::sevastolink(45);
  Sys::IOAPIC::init();
  Drivers::fb::gfx::splash::sevastolink(50);
  LAPIC::init();
  Drivers::fb::gfx::splash::sevastolink(55);
  IRQ::init();
  Drivers::fb::gfx::splash::sevastolink(60);
  
  Sys::Timer::init();
  
  Log::info("Active timer: %s", Sys::Timer::get_timer_name());
  Log::info("Timer frequency: %lld Hz", Sys::Timer::get_frequency());
  Log::info("Initial counter value: %lld", Sys::Timer::get_counter());
  
  Log::info("Testing 100ms sleep...");
  u64 start_ms = Sys::Timer::get_uptime_ms();
  Sys::Timer::sleep_ms(100);
  u64 end_ms = Sys::Timer::get_uptime_ms();
  Log::info("Sleep completed. Elapsed: %lld ms", end_ms - start_ms);
  
  Log::info("Uptime: %lld ms, %lld us, %lld ns", 
            Sys::Timer::get_uptime_ms(), 
            Sys::Timer::get_uptime_us(), 
            Sys::Timer::get_uptime_ns());

  // Purely aesthetic delay
  Sys::Timer::sleep_ms(200);
  Drivers::fb::gfx::splash::sevastolink(75);
  Sys::Timer::sleep_ms(300);
  Drivers::fb::gfx::splash::sevastolink(90);
  
  Sys::ACPI::load();
  Drivers::fb::gfx::splash::sevastolink(100);
}

void _kernel_start() {
  _kernel_pre_setup();
  Log::print("\n      ------------------- \n\n");
  Log::success("Booted from %s %s", boot_ctx.name, boot_ctx.version);
  Log::success("Running Slayer %s  [ %s ]", SLAY_VERSION, boot_ctx.kernel_file->path);

}

extern "C" {

void _start(void) {
  _kernel_start();
  Taskman::start_scheduler();
  
  for (;;) {
    asm("hlt");
  }
}


}

