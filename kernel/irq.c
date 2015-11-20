/*
 * irq.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <asm/io.h>
#include <asm/system.h>

#define is_valid_irq(n) (n>=0 && n<16)

#define INT_CTL         0x20
#define INT_CTLMASK     0x21
#define INT2_CTL        0xA0
#define INT2_CTLMASK    0xA1

static unsigned char cache_21 = 0xFB;   // IRQ 2 on Master is cascaded
static unsigned char cache_A1 = 0xFF;

//IRQ handlers added by add_irq_handler
volatile void *IRQ_TABLE[16];

static void enable_irq(unsigned int irq_nr) {
    // this routine is called by add_irq_handler, so irq_nr
    // is in the range [0 .. 15]. 
    // save_flags(flags); cli(); ... restore_flags(flags) have already
    // been called.
    // unsigned long flags;
    unsigned char mask;

    mask = ~(1 << (irq_nr & 7));
    if (irq_nr < 8) {
        cache_21 &= mask;
        outb(cache_21,0x21);
    } else {
        // here 8 <= irq_nr < 16
        cache_A1 &= mask;
        outb(cache_A1,0xA1);
    }
}

void add_irq_handler(unsigned int irq_nr, void *handler) {
    long flags;

    save_flags(flags); cli();
    if (is_valid_irq(irq_nr)) {
        IRQ_TABLE[irq_nr] = handler;
        enable_irq(irq_nr);
    }
    restore_flags(flags);
}

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

