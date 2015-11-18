/*
 * asm.S
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include "linkage.h"
#include "sys.h"

.extern unhand_int
.extern IRQ_TABLE

/*
 * Save all registers
 */
#define SAVE_ALL	\
	cld;			\
	pusha;			\
	pushw	%ds;		\
	pushw	%es;		\
	pushw	%fs;		\
	pushw	%gs;		\
	movw	$0x10,%si;	\
	movw	%si,%ds;	\
	movw	%si,%es;	\
	pushl	$ret_from_intr

/*
 * The routine used to return from an interrupt. The keyboard_interrupt
 * is a C routine so it return with a RET instruction. An interrupt handler must
 * return with IRET, so this routine is necessary.
 */
ret_from_intr:
    CLI     /* NO INTERRUPT! */
    popw    %gs
    popw    %fs
    popw    %es
    popw    %ds
    popa

    IRET

.extern sys_call_table
ENTRY(system_call)  /* int 0x80: EAX: call_num, parameters in EBX/ECX/EDX */
    SAVE_ALL
    STI     /* interrupts enabled during a system call */

    cmpl    $NR_SYSCALLS-1, %eax    /* exist? */
    ja  1f

    /* parametri = ebx, ecx, edx */
    pushl   %edx
    pushl   %ecx
    pushl   %ebx

    call    *sys_call_table(,%eax,4)
    addl    $3*4, %esp
1:
    RET

ENTRY(_unhand_int)
    SAVE_ALL
    call    unhand_int  /* il codice C provvede... */
    RET

INT_CTL     = 0x20
INT_CTLMASK = 0x21
INT2_CTL    = 0xA0
INT2_CTLMASK    = 0xA1
ENABLE      = 0x20

#define HWINT_MASTER(irq) \
    ENTRY(_hwint##irq)      \
    SAVE_ALL;           \
    inb $INT_CTLMASK, %al;  \
    orb $1<<irq, %al;       \
    outb    %al, $INT_CTLMASK;  \
    movb    $ENABLE, %al;       \
    outb    %al, $INT_CTL;      \
    STI;                \
    call    *(IRQ_TABLE+4*irq); \
    CLI;                \
    inb $INT_CTLMASK, %al;  \
    andb    $~(1<<irq), %al;    \
    outb    %al, $INT_CTLMASK;  \
    RET             /* Get the address of the ret_from_intr
                     * routine from the stack.
                     */

#define HWINT_SLAVE(irq) \
    ENTRY(_hwint##irq)      \
    SAVE_ALL;           \
    inb $INT2_CTLMASK, %al; \
    orb $1<<(irq-8), %al;   \
    outb    %al, $INT2_CTLMASK; \
    movb    $ENABLE, %al;       \
    outb    %al, $INT_CTL;      \
    jmp .+2;            \
    outb    %al, $INT2_CTL;     \
    STI;                \
    call    *(IRQ_TABLE+4*irq); \
    CLI;                \
    inb $INT2_CTLMASK, %al; \
    andb    $~(1<<(irq-8)), %al;    \
    outb    %al, $INT2_CTLMASK; \
    RET

ENTRY(_hwint0)      /* timer handler: do not enable interrupt */
    SAVE_ALL
    call    *(IRQ_TABLE)    /* element 0 */
    movb    $ENABLE, %al
    outb    %al, $INT_CTL

    RET

HWINT_MASTER(1)
HWINT_MASTER(2)
HWINT_MASTER(3)
HWINT_MASTER(4)
HWINT_MASTER(5)
HWINT_MASTER(6)
HWINT_MASTER(7)

HWINT_SLAVE(8)
HWINT_SLAVE(9)
HWINT_SLAVE(10)
HWINT_SLAVE(11)
HWINT_SLAVE(12)
HWINT_SLAVE(13)
HWINT_SLAVE(14)
HWINT_SLAVE(15)
