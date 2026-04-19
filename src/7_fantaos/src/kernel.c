#include <libc/stdint.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <keyboard.h>
#include <terminal.h>

void main(uint32_t magic, void *mbi) {
    (void)magic;
    (void)mbi;

    gdt_init();
    terminal_init();
    idt_init();

    // Test three ISRs (Task 2)
    printf("Testing ISRs...\n");
    asm volatile("int $0x0");  // #DE - Division Error
    asm volatile("int $0x3");  // #BP - Breakpoint
    asm volatile("int $0x6");  // #UD - Invalid Opcode
    printf("ISR test complete.\n");

    // Remap PIC, register keyboard handler, enable interrupts (Task 3 & 4)
    pic_init();
    keyboard_init();
    asm volatile("sti");
    printf("Keyboard ready.\n");

    // Main loop: halt until an interrupt fires, then drain the scancode buffer.
    // keyboard_handler enqueues raw scancodes; keyboard_poll translates and prints.
    for (;;) {
        asm volatile("hlt");
        keyboard_poll();
    }
}
