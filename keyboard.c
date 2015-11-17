#include "io.h"
#include "system.h"

unsigned char *vid_mem = (unsigned char *)(0xb8000);

extern void kb_intr(void);

/*
 * the keyboard interrupt handler. Here it prints a simple "Hello" message.
 */
void keyboard_interrupt(void) {
    *vid_mem++ = 'H';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'e';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'l';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'l';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'o';
    *vid_mem++ = 0x6;
    *vid_mem++ = ' ';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'K';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'e';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'y';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'b';
    *vid_mem++ = 0x6;
	*vid_mem++ = 'o';
	*vid_mem++ = 0x6;
	*vid_mem++ = 'a';
    *vid_mem++ = 0x6;
	*vid_mem++ = 'r';
    *vid_mem++ = 0x6;
	*vid_mem++ = 'd';
	*vid_mem++ = 0x6;
}

/*
 * Initialize the keyboard driver associating to it IRQ1 (interrupt 0x21)
 */
void keyboard_init(void) {
    register unsigned char a;

    // programming the PIC 8259 Master Controller in order to associate IRQ1
    // to the keyboard interrupt handler. Remember the interrupt
    // vector of IRQ1 is 0x21 (see PIC programming in setup.S).
    set_intr_gate(0x21, &kb_intr);
    outb_p(inb_p(0x21) & 0xfd, 0x21);

	// disable/enable the keyboard
    a=inb_p(0x61);
    outb_p(a|0x80, 0x61);
    outb(a, 0x61);
}
