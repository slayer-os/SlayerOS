#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H
#include <libc/types.h>


void log_print(const char *message);
void log_printf(const char *format, ...);
void log_info(const char *format, ...);
void log_critical(const char *format, ...);
void log_failed_assertion(const char *format, ...);
void log_success(const char *format, ...);
void log_running(const char *format, ...);
void log_debug(const char *format, ...);


void __log_failed_assert(const char *assertion, const char *message, const char *file, u32 line, const char *function);

#endif
