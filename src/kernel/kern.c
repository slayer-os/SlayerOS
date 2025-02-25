#include <arch/serial.h>
#include <dbg/log.h>
#include <host/flags.h>
#include <bootloader/limine.h>
#include <mem/frames.h>
#include <mem/paging.h>
#include <libc/assert.h>

void _kernel_pre_setup() {
  serial_init();
  bootloader_gather();
  init_frame_alloc();
  init_paging(); log_success("Paging initialized");
}

#define NOT_ZERO 0

void _kernel_start() {
  _kernel_pre_setup();
  log_print("\n      ------------------- \n\n");
  log_success("Booted from %s %s", boot_ctx.name, boot_ctx.version);
  log_success("Running Slayer %s", SLAY_VERSION);
}

void _start(void) {
  _kernel_start();
  for (;;);
}
