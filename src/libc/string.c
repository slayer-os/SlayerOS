#include <libc/string.h>

void *memcpy(void *dest, const void *src, u32 n) {
    u8 *pdest = (u8 *)dest;
    const u8 *psrc = (const u8 *)src;
 
    for (u32 i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }
 
    return dest;
}
void *memset(void *s, int c, u32 n) {
    u8 *p = (u8 *)s;
    for (u32 i = 0; i < n; i++) {
        p[i] = (u8)c;
    }
    return s;
}
void *memmove(void *dest, const void *src, u32 n) {
    u8 *pdest = (u8 *)dest;
    const u8 *psrc = (const u8 *)src;
 
    if (src > dest) {
        for (u32 i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (u32 i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }
 
    return dest;
}
u32 strlen(const char *str) {
    u32 len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

void reverse_str(char *str, size_t length) {
    size_t start = 0;
    size_t end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

char* itoa(s64 value, char *str, int base) {
    if (base < 2 || base > 36) {
        str[0] = '\0';
        return str;
    }

    const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    s64 num = value;
    size_t i = 0;
    int is_negative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }

    while (num != 0) {
        int rem = num % base;
        str[i++] = digits[rem];
        num /= base;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';
    reverse_str(str, i);

    return str;
}

char* itoa_hex(u64 value, char *str) {
    const char hex_digits[] = "0123456789abcdef";
    size_t i = 0;

    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    while (value != 0) {
        int rem = value & 0xF; // Get the last 4 bits
        str[i++] = hex_digits[rem];
        value >>= 4; // Shift right by 4 bits
    }

    str[i] = '\0';
    reverse_str(str, i);

    return str;
}

u64 atoi(const char *ptr) {
    u64 res = 0;
    for (size_t i = 0; i < strlen(ptr); i++) {
        res = res * 10 + (ptr[i] - '0');
    }
    return res;
}


void strcat(char *dest, const char *src) {
    size_t bpos = 0;
    size_t slen = strlen(src);
    while (dest[bpos]) {
        bpos++;
    }
    memcpy(dest+bpos, src, slen);
    dest[bpos+slen] = '\0';
}

u16 strcmp(const char *str1, const char *str2) {
    if (strlen(str1) != strlen(str2)) {
        return 1;
    }
    for (size_t i = 0; i < strlen(str1); i++) {
        if (str1[i] != str2[i]) {
            return 1;
        }
    }
    return 0;
}

bit memcmp(const void *ptr1, const void *ptr2, u32 n) {
    const u8 *p1 = (const u8 *)ptr1;
    const u8 *p2 = (const u8 *)ptr2;
    for (u32 i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return 1;
        }
    }
    return 0;
}

char *strtok(const char *str, const char *delim) {
    static char *token = NULL;
    if (str) {
        token = (char*)str;
    } else if (!token) {
        return NULL;
    }
    char *start = token;
    token += strspn(token, delim);
    if (!*token) {
        return NULL;
    }
    token += strcspn(token, delim);
    if (*token) {
        *token++ = '\0';
    }
    return start;
}

size_t strspn( const char* dest, const char* src ) {
    size_t count = 0;
    while (*dest && strchr(src, *dest++)) {
        count++;
    }
    return count;
}

size_t strcspn( const char *dest, const char *src ) {
    size_t count = 0;
    while (*dest && !strchr(src, *dest++)) {
        count++;
    }
    return count;
}

const char* strchr( const char* str, int ch ) {
    while (*str) {
        if (*str == ch) {
            return str;
        }
        str++;
    }
    return NULL;
}

void strcpy(char *dest, const char *src) {
    memcpy(dest, src, strlen(src));
    dest[strlen(src)] = '\0';
}
