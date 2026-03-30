#pragma once

#include <libc/stddef.h>

void terminal_init(void);
void terminal_write(const char* s);
void terminal_write_dec(uint32_t n);
void terminal_write_hex(uint32_t n);
static void terminal_putchar(char c);
static void terminal_scroll(void);