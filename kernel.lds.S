#include "config.h"

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SYSADDR = DEF_SYSADDR;

SECTIONS
{
    . = SYSADDR;
    __KERNEL_START__ = .;

    .text : { *(.text) }
    .rodata : { *(.rodata) *(.rodata.*) }
    .data : { *(.data) }
    _BSS_START_ = .;
    .data : { *(.bss) }
    _BSS_END_ = .;

    __KERNEL_END__ = .;
}
