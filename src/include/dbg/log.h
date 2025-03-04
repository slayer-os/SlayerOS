#include <stdarg.h>
#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H
#include <libc/types.h>

namespace Log {
  int print(const char *message);
  int printf(const char *format, ...);
  int lvsprintf(const char *format, va_list args);
  int info(const char *format, ...);
  int critical(const char *format, ...);
  int failed_assertion(const char *format, ...);
  int success(const char *format, ...);
  int running(const char *format, ...);
  int debug(const char *format, ...);
  void __failed_assert(const char *assertion, const char *message, const char *file, u32 line, const char *function);
}
#endif
