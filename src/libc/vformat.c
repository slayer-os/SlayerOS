#include <stdarg.h>
#include <libc/string.h>

void vsprintf(char *buffer, const char *format, va_list args) {
    size_t b_pos=0;
    size_t f_pos=0;
    while (format[f_pos]) {
        if (format[f_pos] == '%') {
            f_pos++;
            if (format[f_pos] == 'd') {
                int i = va_arg(args, int);
                char str[32];
                itoa(i, str, 10);
                memcpy(buffer+b_pos, str, strlen(str));
                b_pos += strlen(str)-1;
            }
            else if (format[f_pos] == 'x') {
                int i = va_arg(args, int);
                char str[32];
                str[0] = '0';
                str[1] = 'x';
                itoa_hex(i, str+2);
                memcpy(buffer+b_pos, str, strlen(str));
                b_pos += strlen(str)-1;
            }
            else if (format[f_pos] == 'p') {
                s64 i = va_arg(args, s64);
                char str[32];
                str[0] = '0';
                str[1] = 'x';
                itoa_hex(i, str+2);
                memcpy(buffer+b_pos, str, strlen(str));
                b_pos += strlen(str)-1;
            }
            else if (format[f_pos] == 's') {
                char *s = va_arg(args, char *);
                memcpy(buffer+b_pos, s, strlen(s));
                b_pos += strlen(s)-1;
            }
            else if (format[f_pos] == 'a') {
                char str[32];
                u8 off = 0;
                int i = va_arg(args, int);
                if (i < 10) {
                    str[0] = '0';
                    off = 1;
                }
                itoa(i, str+off, 10);
                memcpy(buffer+b_pos, str, strlen(str));
                b_pos += strlen(str)-1;
            }
            else if (format[f_pos] == 'c') {
                char c = va_arg(args, int);
                buffer[b_pos] = c;
            }
        } else {
            buffer[b_pos] = format[f_pos];
        }
        b_pos++;
        f_pos++;
    }
    buffer[b_pos] = 0;
}
