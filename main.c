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

_syscall1(int, print, char*, msg)
_syscall0(int, fork)
_syscall1(int, exec, char*, name)

extern void init_traps(void);
extern void init_irq(void);

extern void time_init(void);
extern void sched_init(void);

/*
 * This is the kernel main routine. When the boot process is completed, this
 * function is called.
 */
void start_kernel(void) {
    int pid;

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

    printk("Kernel info: %u bytes, start at %x end at %x\n",
           K_SIZE, K_START, K_END);

    sti();

    // switch in user mode
    move_to_user_mode();

    pid = fork();

    if (pid == 0) {
        exec("PRG1");
    } else if (pid < 0) {
        print("idle: cannot duplicate myself.\n");
    }

    // this message will not be printed since the child process will be always
    // up and running, so there is no chance for idle process to be scheduled.
    print("idle task is running.\n");

    // idle loop
    while(1);
}
