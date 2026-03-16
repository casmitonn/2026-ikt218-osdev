#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include <multiboot2.h>

#include "vga.h"
#include "../../gdt/gdt.h"
#include "libc/string.h"



struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

int kernel_main();


int compute(int a, int b) { // erilagjeorigj
    return a + b;
}

// Example to make assembly to C 
typedef struct{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e[6];
} MyStruct; 



int main(uint32_t myStruct, uint32_t magic, struct multiboot_info* mb_info_addr) {

    // Example to make assembly to C
     MyStruct* myStructPtr = (MyStruct*)myStruct; // liajdrroai
    

    initGdt(); // Initialize Global Descriptor Table
    //ldt_init(); // Initialize Local Descriptor Table

    print_string("Hello World", 0x0F); // Prints "Hello World" to the screen with white text on black background


    // Call cpp kernel_main (defined in kernel.cpp)
    return kernel_main();
}