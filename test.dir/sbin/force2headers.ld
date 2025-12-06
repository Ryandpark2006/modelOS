/* Linker script to force exactly 2 program headers */
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start)

SECTIONS
{
    . = 0x80000000;
    
    .text : {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
    }
    
    .data : {
        *(.data)
        *(.data.*)
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    /* Discard all unwanted sections */
    /DISCARD/ : {
        *(.note.*)
        *(.gnu.*)
        *(.comment)
        *(.eh_frame)
        *(.eh_frame_hdr)
    }
}

