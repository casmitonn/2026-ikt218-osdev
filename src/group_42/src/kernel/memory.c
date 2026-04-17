#include "kernel/memory.h"
#include "kernel/pmm.h"
#include <kernel/log.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

memory_info_t memory_info;

void init_mm(const uint32_t* kernel_end) {
    (void)kernel_end;
    memory_info.last_alloc = 0;
    memory_info.heap_end = 0;
    memory_info.heap_begin = 0;
    memory_info.memory_used = 0;
    memory_info.pheap_begin = 0;
    memory_info.pheap_end = 0;
    memory_info.pheap_desc = NULL;
}

void print_memory_layout() {
    printf("Memory used: %d bytes\n", memory_info.memory_used);
    printf("Memory free: %d KB (from PMM)\n", pmm_get_free_count() * 4);
    printf("Total frames: %d\n", pmm_get_total_count());
    printf("Free frames: %d\n", pmm_get_free_count());
}
