#include <dbg/log.h>
#include <stdarg.h>
#include <klib/string.h>
#include <arch/serial.h>

void print_serial(const char *message) {
  while (*message) {
    UART::write(*message);
    message++;
  }
}

int Log::print(const char *message) {
  print_serial(message);
  // TODO: Print to the screen
  return strlen(message);
}

int Log::printf(const char *format, ...) {
  int out_size = 0;
  va_list args;
  va_start(args, format);
  char buffer[1024];
  ::vsprintf(buffer, format, args);
  out_size += Log::print(buffer);
  va_end(args);
  return out_size;
}

namespace Log {
  int vsprintf(const char *format, va_list args) {
    char buffer[1024];
    ::vsprintf(buffer, format, args);
    return Log::print(buffer);
  }
}

int Log::info(const char *format, ...) {
  int out_size = 0;
  char base[128] = "\x1b[37;1m[    INFO]\x1b[36m ";
  strcat(base, format);
  va_list args;
  va_start(args, format);
  out_size += Log::vsprintf(base, args);
  va_end(args);
  out_size += Log::print("\x1b[0m\n");
  return out_size;
}

int Log::critical(const char *format, ...) {
  int out_size = 0;
  char base[128] = "\x1b[31;1m[CRITICAL]\x1b[36m ";
  strcat(base, format);
  va_list args;
  va_start(args, format);
  out_size += Log::vsprintf(base, args);
  va_end(args);
  out_size += Log::print("\x1b[0m\n");
  return out_size;
}

int Log::success(const char *format, ...) {
  int out_size = 0;
  char base[128] = "\x1b[32;1m[ SUCCESS]\x1b[36m ";
  strcat(base, format);
  va_list args;
  va_start(args, format);
  out_size += Log::vsprintf(base, args);
  va_end(args);
  out_size += Log::print("\x1b[0m\n");
  return out_size;
}

int Log::running(const char *format, ...) {
  int out_size = 0;
  char base[128] = "\x1b[90;1m[ RUNNING]\x1b[36m ";
  strcat(base, format);
  va_list args;
  va_start(args, format);
  out_size += Log::vsprintf(base, args);
  va_end(args);
  out_size += Log::print("\x1b[0m\n");
  return out_size;
}

int Log::debug(const char *format, ...) {
  int out_size = 0;
  char base[128] = "\x1b[34;1m[   DEBUG]\x1b[36m ";
  strcat(base, format);
  va_list args;
  va_start(args, format);
  out_size += Log::vsprintf(base, args);
  va_end(args);
  out_size += Log::printf("\x1b[0m\n");
  return out_size;
}

const char *ASSERT_MESSAGE = "Failed assertion \x1b[0m\x1b[41m%s\x1b[0m\n\t"
                             "   [ \x1b[33m%s\x1b[0m ]  ->  \x1b[36;4m%s:%d\x1b[0m in \x1b[36;4m%s";
void Log::__failed_assert(const char *assertion, const char *message, const char *file, u32 line, const char *function) {
  Log::critical(ASSERT_MESSAGE, assertion, message, file, line, function);
}
