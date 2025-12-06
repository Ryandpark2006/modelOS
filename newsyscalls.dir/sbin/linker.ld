OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start)

PHDRS
{
  text PT_LOAD FLAGS(7);  /* RWX */
  stack PT_GNU_STACK FLAGS(7);  /* RWX */
}

SECTIONS
{
  . = 0x80000000;
  
  .text : { *(.text .text.*) } :text
  .rodata : { *(.rodata .rodata.*) } :text
  .eh_frame : { *(.eh_frame) } :text
  .got.plt : { *(.got.plt) } :text
  .bss : { *(.bss .bss.*) *(COMMON) } :text
  
  /DISCARD/ : {
    *(.note.gnu.property)
    *(.note.GNU-stack)
    *(.gnu.hash)
    *(.dynsym)
    *(.dynstr)
    *(.gnu.version)
  }
}

