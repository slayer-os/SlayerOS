#include "serial_utils.h"
#include "fs.h"
#include <string.h>

void shell_loop(void) {
    const char* message = "SlayerOS Shell v1.0\nType 'help' for commands.\n";
    serial_write(message, strlen(message));

    char buffer[256];

    while (1) {
        const char* prompt = "> ";
        serial_write(prompt, strlen(prompt));

        // Simulated input handling (replace with real keyboard driver later)
        buffer[0] = 'l';
        buffer[1] = 's';
        buffer[2] = '\0';

        if (buffer[0] == '\0') continue;

        if (!strcmp(buffer, "help")) {
            const char* help_message = "Commands: help, ls, cd, echo, clear\n";
            serial_write(help_message, strlen(help_message));
        } else if (!strcmp(buffer, "ls")) {
            fs_list();
        } else if (!strcmp(buffer, "clear")) {
            const char* clear_sequence = "\033[2J\033[H";  // ANSI clear screen
            serial_write(clear_sequence, strlen(clear_sequence));
        } else {
            const char* unknown_message = "Unknown command\n";
            serial_write(unknown_message, strlen(unknown_message));
        }
    }
}