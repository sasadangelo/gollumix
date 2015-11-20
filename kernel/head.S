/*
 * head.S
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <gollumix/tasks.h>
#include <gollumix/segment.h>

INIT_STACK = 0x94000-0x0C

.text
.global idt, gdt

.global stext
.align 4,0x90
stext:

startup_32:
    cld
    movl    $KERNEL_DS,%eax	# DS = ES = FS = GS = entry 0x10 in GDT containing
    movw    %ax,%ds         # reference to data segment
    movw    %ax,%es
    movw    %ax,%ss

    movw    %ax,%fs
    movw    %ax,%gs

    movl    $INIT_STACK, %esp
    call    setup_idt
    call    setup_gdt
    call    start_kernel

done:
    jmp done

# setup_gdt
# This routine reload the GDT with the following format.
#
#  ---------------
# |     NULL      |
# |_______________|
# |  Kernel Code  |
# |_______________|
# |  Kernel Data  |
# |_______________|
# |     TSS(1)    |
# |_______________|
# |     LDT(1)    |
# |_______________|
# |     TSS(2)    |
# |_______________|
# |     LDT(2)    |
# |_______________|
# |               |
# |     ...       |
# |_______________|
# |TSS(NR_TASK-1) |
# |_______________|
# |LDT(NR_TASK-1) |
# |_______________|
setup_gdt:
    lgdt gdt_descr
    ret

# setup_idt
#
#  sets up a idt with 256 entries pointing to
#  ignore_int, interrupt gates. It then loads
#  idt. Everything that wants to install itself
#  in the idt-table may do so themselves. Interrupts
#  are enabled elsewhere, when we can be relatively
#  sure everything is ok. This routine will be over-
#  written by the page tables.
setup_idt:
    lea ignore_int, %edx
    movl $0x00080000, %eax
    movw %dx, %ax        /* selector = 0x0008 = cs */
    movw $0x8E00,%dx    /* interrupt gate - dpl=0, present */

    lea idt, %edi
    mov $256, %ecx
rp_sidt:
    movl %eax, (%edi)
    movl %edx, 4(%edi)
    addl $8,%edi
    dec %ecx
    jne rp_sidt
    lidt idt_descr
    ret

# This is the default interrupt handler.
# Something is put on the screen, so we are sure that something is happening.
# Now a simple 'X' character is printed in the upper left corner.
.align 2
ignore_int:
    movb $'X', 0xb8000
    movb $0x06, 0xb8000+1
    iret

.align 2
.word 0
gdt_descr:
    .word (2*NR_TASKS+3)*8-1 # gdt contains NR_TASK*2+3 entries.
    .long gdt

.align 2
.word 0
idt_descr:
    .word 256*8-1       # idt contains 256 entries
    .long idt

# Initialize the Interrupt Desciptor Table (IDT) with dummy values
idt: .fill 256, 8, 0

# The global descriptor table (see setup.S)
gdt:
    .word   0, 0, 0, 0

    .word   0xFFFF      # 4Gb - (0x100000*0x1000 = 4Gb)
    .word   0           # base address = 0
    .word   0x9A00      # code read/exec
    .word   0x00CF      # granularity = 4096, 386
                        #  (+5th nibble of limit)

    .word   0xFFFF      # 4Gb - (0x100000*0x1000 = 4Gb)
    .word   0           # base address = 0
    .word   0x9200      # data read/write
    .word   0x00CF      # granularity = 4096, 386
                        #  (+5th nibble of limit)
    .fill   2*NR_TASKS,8,0
