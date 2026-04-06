#include "libc/stddef.h"
#include "libc/stdbool.h"
#include <multiboot2.h>
#include "libc/stdint.h"
#include "gdt.h"
#include "terminal.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "keyboard.h"
#include "memory.h"
#include "pit.h"
#include "song/song.h"

extern uint32_t end;

struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

/* Two simple songs */
static Note simple_song_1[] = {
    {C4, 300}, {D4, 300}, {E4, 300}, {F4, 300},
    {G4, 300}, {R, 150},
    {G4, 300}, {F4, 300}, {E4, 300}, {D4, 300},
    {C4, 500}
};

static Note simple_song_2[] = {
    {E4, 250}, {G4, 250}, {A4, 400}, {R, 150},
    {A4, 250}, {G4, 250}, {E4, 400}, {R, 150},
    {D4, 250}, {F4, 250}, {G4, 400}, {R, 150},
    {C4, 500}
};

int main(uint32_t magic, struct multiboot_info* mb_info_addr) {
    (void)magic;
    (void)mb_info_addr;

    gdt_init();
    terminal_initialize();

    idt_init();
    isr_init();
    irq_init();

    init_kernel_memory(&end);
    init_paging();
    print_memory_layout();

    init_pit();
    keyboard_init();

    printf("Hello World\n");
    printf("Interrupt-system aktivert!\n");
    printf("Music player starting...\n");

    void* some_memory = malloc(12345);
    void* memory2 = malloc(54321);
    void* memory3 = malloc(13331);

    printf("Allocated memory blocks:\n");
    printf("some_memory = 0x%x\n", (uint32_t)some_memory);
    printf("memory2     = 0x%x\n", (uint32_t)memory2);
    printf("memory3     = 0x%x\n", (uint32_t)memory3);

    Song songs[] = {
        {simple_song_1, sizeof(simple_song_1) / sizeof(Note)},
        {simple_song_2, sizeof(simple_song_2) / sizeof(Note)}
    };

    uint32_t n_songs = sizeof(songs) / sizeof(Song);
    SongPlayer* player = create_song_player();

    while (true) {
        for (uint32_t i = 0; i < n_songs; i++) {
            printf("Playing song %d...\n", i + 1);
            player->play_song(&songs[i]);
            printf("Finished song %d.\n", i + 1);
            sleep_interrupt(500);
        }
    }

    return 0;
}