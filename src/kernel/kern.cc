#include <arch/serial.h>
#include <arch/cpuid.h>
#include <dbg/log.h>

#include <host/flags.h>

#include <bootloader/limine.h>

#include <mem/frames.h>
#include <mem/paging.h>
#include <mem/heap.h>

#include <klib/string.h>
#include <klib/assert.h>
#include <klib/elf.h>
#include <klib/intx.h>
#include <klib/dwarf.h>
#include <arch/sse.h>

#include <err/handler.h>

#include <drivers/fb_gfx.h>

void _kernel_pre_setup() {
  UART::init();
  boot_ctx.gather();
  Mem::Frame::init();
  Mem::Paging::init(); Log::success("Memory mapped"); Log::success("Paging initialized");
  Err::Handler::init_symbols(); Log::success("Kernel symbols mapped");

  if (SSE::enable() && SSE::test()) Log::success("SSE extensions enabled");
  else Log::critical("Failed to enable SSE");
}

void _graphics_setup() {
  fb::gfx::init();
  fb::gfx::fill(0x1f1f1f);
  fb::gfx::splash::ascii_logo(344, 212, 1, 0xFFFFFF);
}

void _kernel_start() {
  _kernel_pre_setup();
  Log::print("\n      ------------------- \n\n");
  Log::success("Booted from %s %s", boot_ctx.name, boot_ctx.version);
  Log::success("Running Slayer %s  [ %s ]", SLAY_VERSION, boot_ctx.kernel_file->path);

  _graphics_setup();

  Log::debug("Screen width: %d, height: %d", fb::gfx::screen::width(), fb::gfx::screen::height());
  Log::debug("Heap allocated at %p", kmalloc(0x100));
  Log::debug("strcpy is: %s", Err::Handler::addrsym_repr((u64)strcpy));

  Log::debug("Brand: %s", CPUID::cpu_brand());
  Log::debug("cpuid: %s", Err::Handler::addrsym_repr((u64)CPUID::cpu_brand));
}

extern "C" {
  void _start(void) {
    _kernel_start();
    for (;;);
  }
}
