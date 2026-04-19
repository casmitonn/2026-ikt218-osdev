#pragma once

// Registers the IRQ1 handler and unmasks the keyboard IRQ line.
// Must be called after idt_init() and pic_init().
void keyboard_init(void);

// Drains the scancode ring buffer, translates each scancode to ASCII,
// and prints printable characters to the terminal.
// Call repeatedly from the main loop (e.g. after each hlt).
void keyboard_poll(void);
