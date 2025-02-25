#include <dbg/log.h>
#include <stdarg.h>
#include <libc/string.h>
#include <arch/serial.h>

void print_serial(const char *message) {
    while (*message) {
        write_serial(*message);
        message++;
    }
}

void log_print(const char *message) {
    print_serial(message);
    // TODO: Print to the screen
}

void log_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsprintf(buffer, format, args);
    log_print(buffer);
    va_end(args);
}

void log_vsprintf(const char *format, va_list args) {
    char buffer[1024];
    vsprintf(buffer, format, args);
    log_print(buffer);
}

void log_info(const char *format, ...) {
    char base[128] = "\x1b[37;1m[INFO]\x1b[36m ";
    strcat(base, format);
    va_list args;
    va_start(args, format);
    log_vsprintf(base, args);
    va_end(args);
    log_print("\x1b[0m\n");
}

void log_critical(const char *format, ...) {
    char base[128] = "\x1b[31;1m[CRITICAL]\x1b[36m ";
    strcat(base, format);
    va_list args;
    va_start(args, format);
    log_vsprintf(base, args);
    va_end(args);
    log_print("\x1b[0m\n");
}

void log_success(const char *format, ...) {
    char base[128] = "\x1b[32;1m[SUCCESS]\x1b[36m ";
    strcat(base, format);
    va_list args;
    va_start(args, format);
    log_vsprintf(base, args);
    va_end(args);
    log_print("\x1b[0m\n");
}

void log_running(const char *format, ...) {
    char base[128] = "\x1b[90;1m[RUNNING]\x1b[36m ";
    strcat(base, format);
    va_list args;
    va_start(args, format);
    log_vsprintf(base, args);
    va_end(args);
    log_print("\x1b[0m\n");
}

void log_debug(const char *format, ...) {
    char base[128] = "\x1b[34;1m[DEBUG]\x1b[36m ";
    strcat(base, format);
    va_list args;
    va_start(args, format);
    log_vsprintf(base, args);
    va_end(args);
    log_print("\x1b[0m\n");
}

const char *ASSERT_MESSAGE = "Failed assertion \x1b[36;41m%s\x1b[0m\n\t"
                             "   [ \x1b[33m%s\x1b[0m ]  ->  \x1b[36;4m%s:%d\x1b[0m in \x1b[36;4m%s(...)";

void __log_failed_assert(const char *assertion, const char *message, const char *file, u32 line, const char *function) {
  log_critical(ASSERT_MESSAGE, assertion, message, file, line, function);
}
