#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

void memory_init(void);
void* malloc(size_t size);
void free(void* ptr);
void* memset(void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
size_t strlen(const char* s);
char* strcpy(char* dest, const char* src);
int memcmp(const void* s1, const void* s2, size_t n);

#endif