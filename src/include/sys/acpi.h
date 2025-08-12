#pragma once

namespace Sys {
  namespace ACPI {
    void init();
    void load();
    int shutdown();
    bool is_initialized();
  }
}
