#include "screen.h"

static unsigned short* const video_memory = (unsigned short*)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;
static const unsigned char color = 0x0F;

static void screen_scroll(void) {
    if (cursor_y < 25) {
        return;
    }

    int i;
    for (i = 0; i < 24 * 80; i++) {
        video_memory[i] = video_memory[i + 80];
    }

    for (i = 24 * 80; i < 25 * 80; i++) {
        video_memory[i] = ((unsigned short)color << 8) | ' ';
    }

    cursor_y = 24;
}

void screen_clear(void) {
    int i;
    for (i = 0; i < 80 * 25; i++) {
        video_memory[i] = ((unsigned short)color << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

void screen_putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        screen_scroll();
        return;
    }

    if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = 79;
        }

        video_memory[cursor_y * 80 + cursor_x] = ((unsigned short)color << 8) | ' ';
        return;
    }

    video_memory[cursor_y * 80 + cursor_x] = ((unsigned short)color << 8) | c;
    cursor_x++;

    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }

    screen_scroll();
}

void screen_write(const char* str) {
    int i = 0;
    while (str[i]) {
        screen_putc(str[i]);
        i++;
    }
}

void screen_write_hex(unsigned int n) {
    char hex[] = "0123456789ABCDEF";
    int i;

    screen_write("0x");
    for (i = 28; i >= 0; i -= 4) {
        screen_putc(hex[(n >> i) & 0xF]);
    }
}

void screen_write_dec(unsigned int n) {
    char buf[16];
    int i = 0;

    if (n == 0) {
        screen_putc('0');
        return;
    }

    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }

    while (i--) {
        screen_putc(buf[i]);
    }
}