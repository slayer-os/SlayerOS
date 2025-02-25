#ifndef LIBC_ASSERT_H
#define LIBC_ASSERT_H

void __lc_assert(bool condition, const char *assertion, const char *message, const char *file, u32 line, const char *function);
#define assert(condition, message) __lc_assert(condition, #condition, message, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#endif
