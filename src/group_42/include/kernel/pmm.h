#pragma once
#include <stdint.h>

#define PMM_BITMAP_PHYS 0x500000
#define PMM_BLOCK_SIZE 4096

typedef struct {
    uint32_t total_frames;
    uint32_t free_frames;
    uint32_t bitmap_size;
    uint8_t* bitmap;
} pmm_info_t;

void pmm_init(void* mb_info);
uint32_t pmm_alloc_frame(void);
void pmm_free_frame(uint32_t phys_addr);
int pmm_get_free_count(void);
int pmm_get_total_count(void);
