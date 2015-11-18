/*
 * head.S
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

INIT_STACK = 0x94000-0x0C

.text
.global idt

.global stext
.align 4,0x90
stext:

startup_32:
	cld
	movl    $0x10,%eax		# DS = ES = FS = GS = entry 0x10 in GDT containing
	movw    %ax,%ds         # reference to data segment
	movw    %ax,%es
    movw    %ax,%ss

    movw    %ax,%fs
    movw    %ax,%gs

    movl    $INIT_STACK, %esp
    call    setup_idt
    call    start_kernel

done:
    jmp done

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
idt_descr:
    .word 256*8-1       # idt contains 256 entries
    .long idt

# Initialize the Interrupt Desciptor Table (IDT) with dummy values
idt: .fill 256, 8, 0
