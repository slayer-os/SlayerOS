#ifndef FS_H
#define FS_H

#include <stddef.h>  // For size_t

void fs_setup(void);
int fs_create(const char *name);
int fs_read(const char *name, char *buffer, size_t size);
int fs_write(const char *name, const char *data, size_t size);
void fs_list(void);

#endif