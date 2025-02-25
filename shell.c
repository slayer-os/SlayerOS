#include "serial.h"
#include "fs.h"

void shell_loop(void) {
    serial_write("SlayerOS Shell v1.0\nType 'help' for commands.\n");

    char buffer[256];
    
    while (1) {
        serial_write("> ");
        // Simulated input handling (replace with real keyboard driver later)
        buffer[0] = 'l';
        buffer[1] = 's';
        buffer[2] = '\0';

        if (buffer[0] == '\0') continue;

        if (!strcmp(buffer, "help")) {
            serial_write("Commands: help, ls, cd, echo, clear\n");
        } else if (!strcmp(buffer, "ls")) {
            fs_list();
        } else if (!strcmp(buffer, "clear")) {
            serial_write("\033[2J\033[H");  // ANSI clear screen
        } else {
            serial_write("Unknown command\n");
        }
    }
}
