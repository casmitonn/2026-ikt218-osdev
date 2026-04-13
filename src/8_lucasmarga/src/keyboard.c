#include "keyboard.h"
#include "terminal.h"
#include "io.h"
#include "libc/stdint.h"

#define KEYBOARD_BUFFER_SIZE 256

static uint8_t keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint32_t buffer_index = 0;

/* Converts scancodes to ASCII characters */
static const char keyboard_map[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,  '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ',
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0
};

void keyboard_handle_input(void) {
    uint8_t scancode = inb(0x60);

    if (scancode & 0x80) {
        return;
    }

    /* Put scancode in buffer */
    if (buffer_index < KEYBOARD_BUFFER_SIZE) {
        keyboard_buffer[buffer_index++] = scancode;
    }

    /* Convert and print character */
    if (scancode < 128) {
        char c = keyboard_map[scancode];
        if (c != 0) {
            terminal_put_char(c);
        }
    }
}