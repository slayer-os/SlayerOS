#include "fs.h"
#include "serial.h"

// Basic string functions
static int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for ( ; i < n; i++)
        dest[i] = '\0';
    return dest;
}

typedef struct {
    char name[32];
    char data[256];
    int used;
} File;

#define MAX_FILES 16
static File ram_fs[MAX_FILES];

void fs_setup(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        ram_fs[i].used = 0;
    }
}

int fs_create(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!ram_fs[i].used) {
            strncpy(ram_fs[i].name, name, sizeof(ram_fs[i].name) - 1);
            ram_fs[i].name[sizeof(ram_fs[i].name) - 1] = '\0';
            ram_fs[i].used = 1;
            ram_fs[i].data[0] = '\0';
            return 0;
        }
    }
    return -1;
}

int fs_read(const char *name, char *buffer, size_t size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (ram_fs[i].used && strcmp(ram_fs[i].name, name) == 0) {
            strncpy(buffer, ram_fs[i].data, size);
            buffer[size - 1] = '\0';
            return 0;
        }
    }
    return -1;
}

int fs_write(const char *name, const char *data, size_t size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (ram_fs[i].used && strcmp(ram_fs[i].name, name) == 0) {
            strncpy(ram_fs[i].data, data, size < sizeof(ram_fs[i].data) ? size : sizeof(ram_fs[i].data) - 1);
            ram_fs[i].data[sizeof(ram_fs[i].data) - 1] = '\0';
            return 0;
        }
    }
    return -1;
}

void fs_list(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (ram_fs[i].used) {
            serial_puts(ram_fs[i].name);
            serial_puts("\n");
        }
    }
}