#ifndef SCREEN_H
#define SCREEN_H

void screen_clear(void);
void screen_putc(char c);
void screen_write(const char* str);
void screen_write_hex(unsigned int n);
void screen_write_dec(unsigned int n);

#endif