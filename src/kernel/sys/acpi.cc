#include <uacpi/uacpi.h>
#include <uacpi/event.h>
#include <uacpi/sleep.h>
#include <sys/acpi.h>
#include <dbg/log.h>

static bool acpi_initialized = false;

void Sys::ACPI::init() {
  uacpi_status ret = uacpi_initialize(0);
  if (uacpi_unlikely_error(ret)) {
    Log::critical("uacpi_initialize error: %s", uacpi_status_to_string(ret));
    return;
  }

  acpi_initialized = true;
  Log::success("ACPI subsystem initialized");
}

void Sys::ACPI::load() {
  /* uacpi_status ret = uacpi_namespace_load();

  if (uacpi_unlikely_error(ret)) {
    Log::critical("uacpi_namespace_load error: %s", uacpi_status_to_string(ret));
    return;
  }

  ret = uacpi_namespace_initialize();
  if (uacpi_unlikely_error(ret)) {
    Log::critical("uacpi_namespace_initialize error: %s", uacpi_status_to_string(ret));
    return;
  }

  ret = uacpi_finalize_gpe_initialization();
  if (uacpi_unlikely_error(ret)) {
    Log::critical("uACPI GPE initialization error: %s", uacpi_status_to_string(ret));
    return;
  } */   
}

bool Sys::ACPI::is_initialized() {
  return acpi_initialized;
}

int Sys::ACPI::shutdown() {
  uacpi_status ret = uacpi_prepare_for_sleep_state(UACPI_SLEEP_STATE_S5);
  if (uacpi_unlikely_error(ret)) {
    Log::critical("failed to prepare for sleep: %s", uacpi_status_to_string(ret));
    return -1;
  }

  __asm__ volatile ("cli");

  ret = uacpi_enter_sleep_state(UACPI_SLEEP_STATE_S5);
  if (uacpi_unlikely_error(ret)) {
    Log::critical("failed to enter sleep: %s", uacpi_status_to_string(ret));
    return -1;
  }

  return 0;
}
