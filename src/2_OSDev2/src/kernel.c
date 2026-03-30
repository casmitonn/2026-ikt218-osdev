#include <libc/stddef.h>
#include <libc/stdint.h>

#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "kernel_memory.h"
#include "keyboard.h"

extern uint32_t end; // Defined in the linker script, marks the end of the kernel

void main(void) {
    terminal_init();
    terminal_write("Hello, OS World!\n");

    gdt_init();
    terminal_write("GDT initialized successfully.\n");

    idt_init();
    terminal_write("IDT initialized successfully.\n");

    irq_init();
    __asm__ volatile ("sti"); // Enable interrupts
    terminal_write("IRQ initialized successfully.\n");

    init_kernel_memory(&end);
    print_memory_layout();

    void* some_memory = malloc(1024); // Allocate 1 KB of memory
    if (some_memory) {
        terminal_write("Memory allocated successfully.\n");
    } else {
        terminal_write("Memory allocation failed.\n");
    }
    void* memory2 = malloc(2048); // Allocate 2 KB of memory
    void* memory3 = malloc(13331); // Allocate 13 KB of memory

    terminal_write("Memory layout after allocations:\n");
    print_memory_layout();

    keyboard_init();
    terminal_write("Keyboard initialized successfully.\n");

    terminal_write("Kernel initialization complete.\n");
    terminal_write("Type on the keyboard to see output:\n");

    for(;;) {
        // Halt the CPU to save power until the next interrupt
        __asm__ volatile ("hlt");
    }
}