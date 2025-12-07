SECTIONS
{
  . = 0x80000000;
  .text : { *(.text) }
  .rodata : { *(.rodata) }
  .data : { *(.data) }
  .bss : { *(.bss) *(COMMON) }
  /DISCARD/ : { *(.note.gnu.property) *(.note.gnu.build-id) *(.note.ABI-tag) *(.gnu.hash) }
}


