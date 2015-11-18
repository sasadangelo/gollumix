/*
 * asm.S
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#define ALIGN	.align 16,0x90
#define ENTRY(name) \
	.global name; \
	ALIGN; \
	name:

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
 * Entry point for keyboard interrupt handler
 */
ENTRY(kb_intr)
	SAVE_ALL	/* salva tutto! */
	STI
	call keyboard_interrupt
	RET         /* take the return address on top of the stack, that contain
                   ret_from_int take. So this instruction jump to ret_from_intr
                   routine */

/*
 * The routine used to return from an interrupt. The keyboard_interrupt
 * is a C routine so it return with a RET instruction. An interrupt handler must
 * return with IRET, so this routine is necessary.
 */
ret_from_intr:
	CLI		/* NO INTERRUPT! */
	popw	%gs
	popw	%fs
	popw	%es
	popw	%ds
	popa

	IRET
