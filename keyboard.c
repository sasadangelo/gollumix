#include "io.h"
#include "system.h"
#include "kernel.h"

extern void kb_intr(void);

/*
 * the keyboard interrupt handler. Here it prints a simple "Hello" message.
 */
void keyboard_interrupt(void) {
	printk("Hello Keyboard\n");
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
