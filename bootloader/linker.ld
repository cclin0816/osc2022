ENTRY(_start)

SECTIONS
{
    . = 0x80000;
    __bootloader_start = .;
    
    .text : { 
        *(.text.boot) 
        *(.text .text.*)
        . = ALIGN(0x10);
    }

    .rodata : { 
        *(.rodata .rodata.*) 
        . = ALIGN(0x10);
    }

    .data : { 
        *(.data .data.*) 
        . = ALIGN(0x10);
    }
    __bootloader_end = .;

    .bss : {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(0x10);
        __bss_end = .;
    }

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}

__bootloader_sz = (__bootloader_end - __bootloader_start) >> 4;
__bss_offset = __bss_start - __bootloader_start;
__bss_sz = (__bss_end - __bss_start) >> 4;