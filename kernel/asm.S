/*
 * asm.S
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <asm/linkage.h>
#include <gollumix/sys.h>
#include <gollumix/segment.h>

.extern unhand_int
.extern IRQ_TABLE

/*
 * Stack layout in ret_from_syscall:
 *
 *   0(%esp) - %ebx
 *   4(%esp) - %ecx
 *   8(%esp) - %edx
 *   C(%esp) - %esi
 *  10(%esp) - %edi
 *  14(%esp) - %ebp
 *  18(%esp) - %eax
 *  1C(%esp) - %ds
 *  20(%esp) - %es
 *  24(%esp) - %eip
 *  28(%esp) - %cs
 *  2C(%esp) - %eflags
 *  30(%esp) - %oldesp
 *  34(%esp) - %oldss
 */

EBX             = 0x00
ECX             = 0x04
EDX             = 0x08
ESI             = 0x0C
EDI             = 0x10
EBP             = 0x14
EAX             = 0x18
DS              = 0x1C
ES              = 0x20
EIP             = 0x24
CS              = 0x28
EFLAGS          = 0x2C
OLDESP          = 0x30
OLDSS           = 0x34

/* Current process info: */
.extern current
STATE = 0
COUNTER = 4
PRIORITY = 8

/* This variable indicate if the scheduler must be called */
ENTRY(need_schedule) .byte 0

/*
 * Save all registers
 */
#define SAVE_ALL	\
	cld;			\
    pushl   %es; \
    pushl   %ds; \
    pushl   %eax; \
    pushl   %ebp; \
    pushl   %edi; \
    pushl   %esi; \
    pushl   %edx; \
    pushl   %ecx; \
    pushl   %ebx; \
    movl    $KERNEL_DS,%edx; \
    movl    %edx,%ds; \
    movl    %edx,%es; \
    movl    $USER_DS,%edx; \
    movl    %edx, %fs;

#define RESTORE_ALL \
    popl %ebx;  \
    popl %ecx;  \
    popl %edx;  \
    popl %esi;  \
    popl %edi;  \
    popl %ebp;  \
    popl %eax;  \
    popl %ds;   \
    popl %es;   \
    iret;

.extern sys_call_table
.extern schedule
ENTRY(system_call)  /* int 0x80: EAX: call_num, parameters in EBX/ECX/EDX */
    SAVE_ALL
    STI     /* interrupts enabled during a system call */

    cmpl    $NR_SYSCALLS, %eax    /* exist? */
    jae ret_from_syscall
    call    *sys_call_table(,%eax,4)
    movl    %eax, EAX(%esp)         /* get from the stack the return code */

/*
 * The routine used to return from a sys call or interrupt. An interrupt at the
 * difference of a normal routine must return with an IRET instruction (see
 * RESTORE_ALL).
 */
ENTRY(ret_from_syscall)
    CLI                      /* NO INTERRUPT! */

    movl current, %ebx       /* if current == NULL call the scheduler */
    cmpl  $0, %ebx
    je  0f

    cmpl  $0, STATE(%ebx)    /* if the process is not running, schedule */
    jne  0f

    cmpw    $(KERNEL_CS), CS(%esp) 
                             /* if the process is in kernel mode do not sched */
    je  1f

    cmpb $0, need_schedule   /* if it is not necessary to schedule */
    je  1f

0:
    call schedule
1:
    RESTORE_ALL

ENTRY(_unhand_int)
    SAVE_ALL
    call    unhand_int  /* il codice C provvede... */
    jmp ret_from_syscall

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
    jmp ret_from_syscall;          
                                   /* Get the address of the ret_from_intr
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
    jmp ret_from_syscall;

ENTRY(_hwint0)      /* timer handler: do not enable interrupt */
    SAVE_ALL
    call    *(IRQ_TABLE)    /* element 0 */
    movb    $ENABLE, %al
    outb    %al, $INT_CTL
    jmp ret_from_syscall;

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

#define _ex_common(n) \
    SAVE_ALL;               \
    pushl   $0x##n;         \
    call    c_exception;    \
    addl    $8, %esp;       \
    jmp     ret_from_syscall;

#define EXCEPTION(n) \
    ENTRY(_exception0x##n)  \
    _ex_common(n)

#define ERR_EXCEPTION(n) \
    ENTRY(_exception0x##n)  \
    addl    $4, %esp;       \
    _ex_common(n)

EXCEPTION(00)
EXCEPTION(01)
EXCEPTION(02)
EXCEPTION(03)
EXCEPTION(04)
EXCEPTION(05)
EXCEPTION(06)
EXCEPTION(07)
ERR_EXCEPTION(08)
EXCEPTION(09)
ERR_EXCEPTION(0a)
ERR_EXCEPTION(0b)
ERR_EXCEPTION(0c)
ERR_EXCEPTION(0d)
ERR_EXCEPTION(0e)
ENTRY(_exception0x0f)
        jmp _unhand_int
EXCEPTION(10)
