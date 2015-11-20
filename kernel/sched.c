/*
 * sched.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <gollumix/types.h>
#include <gollumix/stddef.h>
#include <gollumix/time.h>
#include <gollumix/sched.h>
#include <gollumix/mm.h>
#include <gollumix/kernel.h>
#include <asm/string.h>
#include <gollumix/kernel_map.h>

#define is_valid_task(pid) ((pid)>0 && (pid)<NR_TASKS && task[pid])

volatile struct timeval xtime;

union task_union {
    struct task_struct task;
    char stack[PAGE_SIZE];
};

// The init task
union task_union init_task = {INIT_TASK, };

// The process table
struct task_struct *task[NR_TASKS] = {&(init_task.task), };

// The current task
struct task_struct *current = &(init_task.task);

/*
 * This is the scheduling routine. The kernel calls this routine each time
 * it move from kernel mode to user mode.
 * This routine should be executed with interrupts disabled.
 */
asmlinkage void schedule(void) {
    int i;
    int next;          // the index of the next task to execute in the process
                       // table
    int c;             // the dynamic priority of the task e are analyzing
    struct task_struct **p; // the process table

    while(1) {
        c = -1;
        next = 0;
        i = NR_TASKS;
        p = &task[NR_TASKS];

        // loop through the process table to find a valid entry
        // task[0] (idle task) is not considered
        while(--i) {
            if(!*--p) {
                continue;
            }

            // if the i process is in running state and its dynamic
            // priority (c) is greater than the previous selected,
            // the it will be the next task to execute.
            if ((*p)->state == TASK_RUNNING && (*p)->counter > c) {
                c = (*p)->counter, next = i;
            }
        }

        if (c) {
            // if c>0, we found a task to run, exit from the loop.
            // if c==-1, then only the idle task is available, so it will
            // be the next task, but at each tick the schedule routine is
            // called again in order to schedule a task.
            break;
        }

        // All the tasks in the process table either are not running or their
        // timeslice is expired. Recalculate the timeslice (the counter field)
        // for them (except for idle task).

        // Loop through the process table
        for(p = &LAST_TASK; p > &FIRST_TASK; --p) {
            // for a valid entry, re calculate the counter field.
            if (*p) {
                (*p)->counter = ((*p)->counter>>1) + (*p)->priority;
            }
        }
    }

    // reset the need_schedule variable
    need_schedule = 0;

    // switch to the next task
    switch_to(task[next]);
}

/*
 * This routine set the task[0]=idle task to the current process.
 */
void sched_init(void) {
    current = task[0];
    set_tss_desc(gdt+FIRST_TSS_ENTRY, &(init_task.task.tss));
    set_ldt_desc(gdt+FIRST_LDT_ENTRY, &(init_task.task.ldt), 3);

    ltr(0);
    lldt(0);
}
