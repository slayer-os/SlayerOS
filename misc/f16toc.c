#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s font.F16 > tty_font.cc\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Failed to open font file");
        return 1;
    }

    unsigned char font[4096];
    size_t read = fread(font, 1, sizeof(font), f);
    fclose(f);

    if (read != sizeof(font)) {
        fprintf(stderr, "Error: expected 4096 bytes, read %zu\n", read);
        return 1;
    }

    printf("#include <klib/types.h>\nextern \"C\" const u8 tty_font[] = {\n");

    for (size_t i = 0; i < sizeof(font); i++) {
        if (i % 12 == 0)
            printf("  ");

        printf("0x%02x", font[i]);

        if (i + 1 < sizeof(font))
            printf(", ");
        else
            printf(" ");

        if ((i + 1) % 12 == 0)
            printf("\n");
    }

    if (sizeof(font) % 12 != 0)
        printf("\n");

    printf("};\n");

    return 0;
}
