#include "keyboard.h"
#include "irq.h"
#include "ports.h"
#include "screen.h"

static char keyboard_buffer[256];
static int keyboard_index = 0;

static const char scancode_to_ascii[128] = {
    0,   27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0,   'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0,  '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0,   '*',
    0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0
};

static void keyboard_handler(struct registers* regs) {
    (void)regs;

    unsigned char scancode = inb(0x60);

    if (scancode & 0x80) {
        return;
    }

    char c = scancode_to_ascii[scancode];
    if (c) {
        if (keyboard_index < 255) {
            keyboard_buffer[keyboard_index++] = c;
            keyboard_buffer[keyboard_index] = '\0';
        }
        screen_putc(c);
    }
}

void keyboard_install(void) {
    irq_install_handler(1, keyboard_handler);
}