#include "pic.h"
#include "io.h"

/* Ports used for PIC communication */
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

/* End of interrupt signal */
#define PIC_EOI      0x20

void pic_remap(void) {
    unsigned char mask1 = inb(PIC1_DATA);
    unsigned char mask2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();

    /* Change interrupt offsets for the PIC */
    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    /* Set up the master PIC to use IRQ2 for the slave */
    outb(PIC1_DATA, 0x04);
    io_wait();

    /* Set the slave PIC's cascade identity */
    outb(PIC2_DATA, 0x02);
    io_wait();

    /* Set 8086 mode */
    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

    /* Restore masks */
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);

    /* Unmask IRQ0 (timer) */
    outb(PIC1_DATA, inb(PIC1_DATA) & ~0x01);
}

void pic_send_eoi(int irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}