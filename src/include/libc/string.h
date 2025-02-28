#include <stdarg.h>
#ifndef LIBC_STRING_H
#define LIBC_STRING_H
#include <libc/types.h>

void *memcpy(void *dest, const void *src, u32 n);
void *memset(void *s, int c, u32 n);
void *memmove(void *dest, const void *src, u32 n);
u32 strlen(const char *str);
char* itoa(s64 value, char *str, int base);
char* itoa_hex(u64 value, char *str);
u64 atoi(const char *ptr);
void strcat(char *dest, const char *src);
void vsprintf(char *buffer, const char *format, va_list args);
void sprintf(char *buffer, const char *format, ...);
u16 strcmp(const char *str1, const char *str2);
bit memcmp(const void *ptr1, const void *ptr2, u32 n);
char *strtok(const char *str, const char *delim);
size_t strspn( const char* dest, const char* src );
size_t strcspn( const char *dest, const char *src );
const char* strchr( const char* str, int ch );
void strcpy(char *dest, const char *src);
//char *strdup(const char *str);

#endif
