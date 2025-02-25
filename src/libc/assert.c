#include <dbg/log.h>

void __lc_assert(bool condition, const char *assertion, const char *message, const char *file, u32 line, const char *function) {
    if (!condition) {
        __log_failed_assert(assertion, message, file, line, function);
        for(;;);
        // TODO: Kernel panic
    }
}
