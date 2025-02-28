#include <arch/serial.h>
#include <dbg/log.h>

#include <host/flags.h>

#include <bootloader/limine.h>

#include <mem/frames.h>
#include <mem/paging.h>
#include <mem/heap.h>

#include <libc/assert.h>
#include <libc/elf.h>

#include <err/handler.h>

#include <drivers/fb_gfx.h>

void _kernel_pre_setup() {
  serial_init();
  bootloader_gather();
  init_frame_alloc();
  init_paging(); log_success("Memory mapped"); log_success("Paging initialized");
  init_symbol_table();
}

void _graphics_setup() {
  init_fb_gfx();
  gfx_fill(0x1f1f1f);
  draw_ascii_logo(344, 212, 1, 0xFFFFFF);
}

void _kernel_start() {
  _kernel_pre_setup();
  log_print("\n      ------------------- \n\n");
  log_success("Booted from %s %s", boot_ctx.name, boot_ctx.version);
  log_success("Running Slayer %s  [ %s ]", SLAY_VERSION, boot_ctx.kernel_file->path);

  _graphics_setup();

  log_debug("Screen width: %d, height: %d", gfx_screen_width(), gfx_screen_height());
  log_debug("Heap allocated at %p", kmalloc(0x100));

  log_debug("_graphics_setup+0x10 is: %s", resolve_address((u64)_graphics_setup+0x10));
}

void _start(void) {
  _kernel_start();
  for (;;);
}
