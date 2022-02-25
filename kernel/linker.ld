ENTRY(_start)

SECTIONS
{
    . = 0x80000;
    __linker_start = .;
    
    .text : { 
        *(.text.boot) 
        *(.text .text.*)
        . = ALIGN(0x1000);
    }

    .rodata : { 
        *(.rodata .rodata.*) 
        . = ALIGN(0x10);

        __vtable_start = .;
        *(.got)
        *(.data.rel.ro .data.rel.ro.*)
        . = ALIGN(0x10);
        __vtable_end = .;
        . = ALIGN(0x1000);
    }

    .data : { 
        *(.data .data.*) 
        . = ALIGN(0x1000);
    }

    .bss : {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(0x1000);
        __bss_end = .;
    }

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}

__bss_sz = (__bss_end - __bss_start);
__vtable_offset = __vtable_start - __linker_start;
__vtable_sz = (__vtable_end - __vtable_start) >> 3;