#include "tty.h"
#include "system.h"
#include "kernel.h"

/*
 * This is the kernel main routine. When the boot process is completed, this
 * function is called.
 */
void start_kernel(void) {
    // initialize the tty driver. The tty is composed by a keyboard driver
    // that read input keys a print the relative ASCII code on video.
    tty_init();
    sti();
    // idle loop
    while(1);
}
