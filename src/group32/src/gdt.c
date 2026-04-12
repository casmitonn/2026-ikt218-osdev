#include "gdt.h"

struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

static struct gdt_entry gdt[3];
static struct gdt_ptr gp;

extern void gdt_flush(unsigned int gp_address);

static void gdt_set_gate(
    int num,
    unsigned int base,
    unsigned int limit,
    unsigned char access,
    unsigned char gran
) {
    gdt[num].base_low = (unsigned short)(base & 0xFFFF);
    gdt[num].base_middle = (unsigned char)((base >> 16) & 0xFF);
    gdt[num].base_high = (unsigned char)((base >> 24) & 0xFF);

    gdt[num].limit_low = (unsigned short)(limit & 0xFFFF);
    gdt[num].granularity = (unsigned char)((limit >> 16) & 0x0F);

    gdt[num].granularity |= (unsigned char)(gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install(void) {
    gp.limit = (unsigned short)((sizeof(struct gdt_entry) * 3) - 1);
    gp.base = (unsigned int)&gdt;

    /* 0: NULL descriptor */
    gdt_set_gate(0, 0, 0, 0, 0);

    /* 1: Code segment */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* 2: Data segment */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    gdt_flush((unsigned int)&gp);
}