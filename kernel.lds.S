#include "config.h"

OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(_start)

SYSADDR = DEF_SYSADDR;

SECTIONS
{
    . = SYSADDR;
    __KERNEL_START__ = .;

    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    _BSS_START_ = .;
    .data : { *(.bss) *(COMMON) }
    _BSS_END_ = .;

    . = ALIGN(4096);
    __KERNEL_END__ = .;
}
