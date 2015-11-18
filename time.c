/*
 * time.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "kernel.h"
#include "io.h"
#include "irq.h"
#include "sched.h"

#define TIMER_COUNT ((unsigned) (TIMER_FREQ/HZ))
#define TIMER_FREQ  1193182L

#define TIMER_MODE  0x43    /* I/O port for timer mode control */
#define TIMER_0     0x40    /* timer channel 0 */
#define SQUARE_WAVE 0x36

unsigned long volatile jiffies = 0;

static void timer_interrupt(void) {
    ++jiffies;
    printk(".");
}

static void setup_timer(void) {
    printk ("Timer:");
    outb_p (SQUARE_WAVE, TIMER_MODE);
    outb_p ((unsigned char)TIMER_COUNT, TIMER_0);
    outb_p ((unsigned char)(TIMER_COUNT>>8), TIMER_0);

    add_irq_handler (0, &timer_interrupt);
    printk (" ok!\n");
}

void time_init(void) {
	setup_timer();
}
