#include "fs.h"
#include "serial.h"
#include <string.h>

typedef struct {
    char name[32];
    char data[256];
    int used;
} File;

#define MAX_FILES 16
static File ram_fs[MAX_FILES];

void fs_setup(void) {
    memset(ram_fs, 0, sizeof(ram_fs));
}

int fs_create(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!ram_fs[i].used) {
            strcpy(ram_fs[i].name, name);
            ram_fs[i].used = 1;
            return 0;
        }
    }
    return -1;
}

int fs_read(const char *name, char *buffer, size_t size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (ram_fs[i].used && !strcmp(ram_fs[i].name, name)) {
            strncpy(buffer, ram_fs[i].data, size);
            return 0;
        }
    }
    return -1;
}

int fs_write(const char *name, const char *data, size_t size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (ram_fs[i].used && !strcmp(ram_fs[i].name, name)) {
            strncpy(ram_fs[i].data, data, size);
            return 0;
        }
    }
    return -1;
}

void fs_list(void) {
    serial_write("Files in RAM:\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (ram_fs[i].used) {
            serial_write(ram_fs[i].name);
            serial_write("\n");
        }
    }
}
