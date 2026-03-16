#define VGA_TEXT_BUFFER 0xb8000u

void print_string(const char *s, unsigned char attrib)
{
    volatile unsigned short *vp = (unsigned short *)VGA_TEXT_BUFFER;
    while (*s)
        *vp++ = (attrib << 8) | *s++;
}
