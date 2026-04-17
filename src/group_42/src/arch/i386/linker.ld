ENTRY(_start)

SECTIONS {
    . = 1M;

    .boot :
    {
        *(.multiboot_header)
    }

    .page_dir 0x00104000 :
    {
        _page_dir = .;
        . = . + 4096;
    }

    .page_tables 0x00108000 :
    {
        _page_tables = .;
        . = . + 8192;
    }

    .text :
    {
        *(.text .text.*)
    }

    .rodata :
    {
        *(.rodata .rodata.*)
    }

    .data :
    {
        *(.data .data.*)
    }

    .bss :
    {
        *(.bss .bss.*)
    }

	end = .; _end = .; __end = .;
}
