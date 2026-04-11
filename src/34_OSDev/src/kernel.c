#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "irq.h"

void main(uint32_t mb_magic, void* mb_info) {
    (void)mb_magic; //stop warnings about unused variables
    (void)mb_info;

    terminal_initialize();
    init_gdt();
    init_idt();
    init_irq();

    __asm__ volatile ("sti"); //enable interrupts after PIC is remapped

    //asm("int $0x1"); //manually trigger interrupt

    printf("Hello World\n");

    while (1) {} //loop forever (kernel should never return so CPU doesnt execute random memory)
}