/*
 * main.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "tty.h"
#include "system.h"
#include "kernel.h"

extern void init_traps(void);
extern void init_irq(void);

extern void time_init(void);

#ifdef DEBUG
#define NUM_PAGES 3
#include "mm.h"

static void test_mm(void) {
    int i;
    char *address[NUM_PAGES];

    // allocate 3 pages
    for (i=0; i<NUM_PAGES; ++i) {
       address[i] = get_free_page();
    }

    // free the pages previously allocated
    for (i=0; i<NUM_PAGES; ++i) {
       free_page(address[i]);
    }
}
#endif

/*
 * This is the kernel main routine. When the boot process is completed, this
 * function is called.
 */
void start_kernel(void) {
    // init traps
    init_traps();
    // initialize IRQs
    init_irq();

    // initialize the tty driver. The tty is composed by a keyboard driver
    // that read input keys a print the relative ASCII code on video.
    tty_init();
    // initialize the timer.
    time_init();
    sti();

#ifdef DEBUG
    // test memory management
    test_mm();
#endif
    
    // idle loop
    while(1);
}
