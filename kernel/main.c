/*
 * main.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <gollumix/tty.h>
#include <asm/system.h>
#include <gollumix/kernel.h>
#include <gollumix/kernel_map.h>
#include <gollumix/sched.h>
#include <gollumix/unistd.h>
#include <gollumix/serial.h>
#include <gollumix/console.h>

extern inline _syscall1(int, print, char*, msg)
extern inline _syscall0(int, fork)
extern inline _syscall1(int, exec, char*, name)

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
    int pid, i;
    char *names[N_CONSOLES] = { "PRG1", "PRG2", "PRG3", 
                                "PRG4", "PRG5", "PRG6" };

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

    // for a process for each console and execute the program PRGi
    for (i=0; i<N_CONSOLES; ++i) {
        // spawn process i and run PRG1
        pid = fork();

        if (pid == 0) {
            exec(names[i]);
        } else if (pid < 0) {
            print("idle: cannot duplicate myself.\n");
        }
    }
    
	print("Idle: ok!\n");

    // idle loop
    while(1);
}
