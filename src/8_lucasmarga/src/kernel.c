#include "libc/stdint.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"

void kmain(uint32_t magic, void* mb_info_addr) {
    (void)magic;
    (void)mb_info_addr;

    gdt_setup();
    terminal_initialize();

    pic_remap();
    idt_setup();

    terminal_print_string("Hello World\n");

    /* Test software interrupts */
    __asm__ __volatile__("int $0");
    __asm__ __volatile__("int $1");
    __asm__ __volatile__("int $2");

    terminal_print_string("Starting handling interrupts...\n");

    /* Enable hardware interrupts */
    __asm__ __volatile__("sti");

    while (1) {
        __asm__ __volatile__("hlt");
    }
}