/*
 * main.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "tty.h"
#include "system.h"
#include "kernel.h"
#include "kernel_map.h"
#include "sched.h"
#include "unistd.h"
#include "serial.h"
#include "console.h"

_syscall1(int, print, char*, msg)
_syscall0(int, fork)
_syscall1(int, exec, char*, name)

extern void init_traps(void);
extern void init_irq(void);

extern void time_init(void);
extern void sched_init(void);

extern unsigned int rs_buffer[RS_QUEUE_MAX];
extern volatile unsigned int rs_bufferin;
extern unsigned int rs_bufferout;

// active wait on buffer to get data coming from serial line.
// This method will be removed in next article.
void rs_loop(void) {
    unsigned int ch;

    // idle loop
    while(1) {
        cli();
        if (rs_bufferin != rs_bufferout) {
            ch = rs_buffer[rs_bufferout++];
            rs_bufferout %= RS_QUEUE_MAX;

            switch(ch) {
            case 13:
                break;
            default:
                con_write2(5, (unsigned char *)&ch, sizeof(unsigned char));
            }
        }
        sti();
    }
}

/*
 * This is the kernel main routine. When the boot process is completed, this
 * function is called.
 */
void start_kernel(void) {
    int pid, i;

    // init traps
    init_traps();
    // initialize IRQs
    init_irq();

    // initialize the tty driver. The tty is composed by a keyboard driver
    // that read input keys a print the relative ASCII code on video.
    tty_init();
    // initialize the timer.
    time_init();
    // initialize the scheduler
    sched_init();

    printk("Kernel info: %u bytes, start at 0x%x0 end at 0x%x0.\n",
           K_SIZE, K_START, K_END);

    sti();

    // the idle process consume in kernel mode the characters coming on
    // serial line. This code will be removed when a real TTY layer is added.
    //rs_loop();

    // switch in user mode
    move_to_user_mode();

    // spawn process 1 and run PRG1
    pid = fork();

    if (pid == 0) {
        exec("PRG1");
    } else if (pid < 0) {
        print("idle: cannot duplicate myself.\n");
    }

    print("idle task is running.\n");

    // idle loop
    while(1) {
        print("idle task is running now. Press a key to restart PRG1 ...\n");
        for (i=0; i<100000000; ++i);
    }
}
