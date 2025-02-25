#include <arch/serial.h>
#include <dbg/log.h>
#include <host/flags.h>
#include <libc/assert.h>


void _kernel_pre_setup() {
  serial_init();
}

void _kernel_start() {
  _kernel_pre_setup();
  log_debug("Slayer ver: %s", SLAY_VERSION);
  assert(SLAY_VERSION == "00000", "Mismatching version");
}

void _start(void) {
  _kernel_start();
  for (;;);
}
