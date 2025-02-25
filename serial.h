#ifndef SERIAL_H
#define SERIAL_H

void serial_init(void);
void serial_puts(const char *s);
void serial_write_char(char c);

#endif