#include "libc/libs.h"
#include <multiboot2.h>
#include "gdt.h"
#include "kernel/memory.h"
#include "kernel/pit.h"
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "interrupts/keyboard.h"

extern uint32_t end;

struct multiboot_info
{
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

int kernel_main();

int main(uint32_t magic, struct multiboot_info *mb_info_addr)
{
    void *memory1;
    void *memory2;
    void *memory3;
    void *memory4;
    int heap_ok;
    uint32_t busy_before;
    uint32_t busy_after;
    uint32_t interrupt_before;
    uint32_t interrupt_after;

    terminal_initialize();

    (void)mb_info_addr;

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        printf("Invalid multiboot magic\n");
        return 1;
    }

    init_gdt();
    
    // Initialize IDT
    init_idt();
    
    // Initialize Interrupts and IRQs
    init_isr();
    init_irq();

    init_kernel_memory(&end);
    init_paging();
    print_memory_layout();

    // Initialize the keyboard logger
    init_keyboard();
    init_pit();

    // Enable CPU Interrupts
    asm volatile("sti");

    printf("Hello world!\n");
    printf("Memory and PIT are ready.\n");

    memory1 = malloc(12345);
    memory2 = malloc(54321);
    memory3 = malloc(13331);
    free(memory2);
    memory4 = malloc(4096);

    heap_ok = memory1 != 0 && memory2 != 0 && memory3 != 0 && memory4 != 0;
    printf("Heap self-test: %d\n", heap_ok);

    busy_before = pit_get_ticks();
    sleep_busy(50);
    busy_after = pit_get_ticks();

    interrupt_before = pit_get_ticks();
    sleep_interrupt(50);
    interrupt_after = pit_get_ticks();
    printf("PIT sleep ticks: %d %d\n",
           (int)(busy_after - busy_before),
           (int)(interrupt_after - interrupt_before));
    printf("CLI ready. Type help.\n");
    keyboard_print_prompt();

    // Call cpp kernel_main (defined in kernel.cpp)
    return kernel_main();
}
