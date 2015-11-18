/*
 * irq.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "io.h"

#define INT_CTL         0x20
#define INT_CTLMASK     0x21
#define INT2_CTL        0xA0
#define INT2_CTLMASK    0xA1

static unsigned char cache_21 = 0xFB;   // IRQ 2 on Master is cascaded
static unsigned char cache_A1 = 0xFF;

void init_irq(void) {
    outb(0xff, INT_CTLMASK);      // OCW1 command. Mask all the IRQ on Master
    outb(0xff, INT2_CTLMASK);     // OCW1 command. Mask all the IRQ on Slave

    // set up PIC
    outb_p(0x11, INT_CTL);        // ICW1 command. 
    outb_p(0x11, INT2_CTL);       // ICW4 will be sent and size of entry in
	                              // interrupt vector is 4 bytes.
	
    outb_p(0x20, INT_CTLMASK);    // ICW2 command.
    outb_p(0x28, INT2_CTLMASK);   // IRQ0 -> 20h, IRQ1 -> 21h, and so on
                                  // IRQ8 -> 28h, IRQ9 -> 29h, and so on	

    outb_p (0x04, INT_CTLMASK);   // ICW3 command.
    outb_p (0x02, INT2_CTLMASK);  // The controller master is attached to
	                              // the slave through IRQ12 (IRQ4 on slave).
                                  // The controller slave is attached to
                                  // the master through IRQ2.

    outb_p (0x01, INT_CTLMASK);   // ICW4 command.
    outb_p (0x01, INT2_CTLMASK);

    // udelay(100);

    // restore masks. OCW1 command.
    outb(cache_21, INT_CTLMASK);  // Mask all interrupt for controller Slave
    outb(cache_A1, INT2_CTLMASK); // Mask all interrupt except IRQ2
}

