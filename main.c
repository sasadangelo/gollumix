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

extern char __KERNEL_END__;

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

    printk("Kernel info: %u bytes, start at 0x%x0 end at 0x%x0.\n",
           &__KERNEL_END__-K_START, K_START, &__KERNEL_END__);

    sti();

    // switch in user mode
    move_to_user_mode();

    // spawn process 1 and run PRG1
    pid = fork();

    if (pid == 0) {
        exec("PRG1");
    } else if (pid < 0) {
        print("idle: cannot duplicate myself.\n");
    }

    // spawn process 2 and run PRG2
    pid = fork();

    if (pid == 0) {
        exec("PRG2");
    } else if (pid < 0) {
        print("idle: cannot duplicate myself.\n");
    }

    // spawn process 3 and run PRG3
    pid = fork();

    if (pid == 0) {
        exec("PRG3");
    } else if (pid < 0) {
        print("idle: cannot duplicate myself.\n");
    }

    // spawn process 4 and run PRG4
    pid = fork();

    if (pid == 0) {
        exec("PRG4");
    } else if (pid < 0) {
        print("idle: cannot duplicate myself.\n");
    }   

    // spawn process 5 and run PRG5
    pid = fork();

    if (pid == 0) {
        exec("PRG5");
    } else if (pid < 0) {
        print("idle: cannot duplicate myself.\n");
    }

    // spawn process 6 and run PRG6
    pid = fork();

    if (pid == 0) {
        exec("PRG6");
    } else if (pid < 0) {
        print("idle: cannot duplicate myself.\n");
    }

    print("Idle: ok!.\n");

    // idle loop
    while(1);
}
