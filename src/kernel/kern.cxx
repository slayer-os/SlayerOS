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

void u512_tests() {
  u512 x = 0x1234567890abcdef;
  u512 y = 0x1234567890abcdef;
  u512 z = x + y;
  u512 w = x * y;

  assert (x == y, "x != y");
  assert (z == 0x2468acf121579bde_u512, "z != 0x2468acf13579bde");
  assert (w == 0x14b66dc328828bca6475f09a2f2a521_u512, "w != 0x14b66dc328828bca6475f09a2f2a521");
  Log::success("[u512] (Arithmetic) OK");

  u512 a = 0x1234567890abcdef;
  u512 b = 0x1234567890abcdeffedcba0987654321_u512;
  u512 q = b / a;
  u512 c = b % a;
  u512 e = u512::expmod(a, b, 0x123456789abcdef0123456_u512);
  assert(q == 0x1000000000000000d_u512, "q != 0x1000000000000000d");
  assert(c == 0x123455ea2eabcdfe, "c != 0x123455ea2eabcdfe");
  Log::debug("e: %#qx", e);
  Log::info("[u512] (Modular) TODO");
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

  u512_tests();
}

extern "C" {
  void _start(void) {
    _kernel_start();
    for (;;);
  }
}
