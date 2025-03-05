#include <arch/serial.h>
#include <dbg/log.h>

#include <host/flags.h>

#include <bootloader/limine.h>

#include <mem/frames.h>
#include <mem/paging.h>
#include <mem/heap.h>

#include <libc/string.h>
#include <libc/assert.h>
#include <libc/elf.h>
#include <libc/intx.h>

#include <err/handler.h>

#include <drivers/fb_gfx.h>


namespace SSE {
  u8 enable();
  bool test();
}
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
  Log::debug("strcpy is: %s", Err::Handler::resolve_address((u64)strcpy));
}

extern "C" {
  void _start(void) {
    _kernel_start();
    for (;;);
  }
}
