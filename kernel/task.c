/*
 * task.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <gollumix/task.h>
#include <gollumix/kernel.h>
#include <gollumix/stddef.h>
#include <gollumix/sched.h>

// the current task
struct task_struct *current;
// the tasks table
struct task_struct *task[NR_TASKS];

// wake up a task sleeping in interruptible state
void wake_up(struct wait_queue *queue) {
    struct task_struct *task;
    struct wait_entry  *entry;

    // if the input queue is invalid, panic!!!!
    if (!queue) {
        panic("tm: wake up on a null wait queue.\n");
    }

    // if the queue is empty, return immediately.
    if (!queue->head) {
        return;
    }

    entry = queue->head;

    // for each entry in the wait queue, put the task state to TASK_RUNNING
    // and if necessary force a rescheduling.
    do {
        task = entry->task;

        if (!task) {
            panic("tm: wait queue entry with null task.\n");
        }

        if (task->state == TASK_INTERRUPTIBLE) {
            task->state = TASK_RUNNING;
            // if the task has a dynamic priority greater than the current task,
            // force a scheduling.
            if (task->counter > current->counter) {
                need_schedule = 1;
            }
        }

        entry = entry->next;
    } while (entry != queue->head);
}

// sleep in interruptible state
void sleep(struct wait_queue *queue) {
    struct wait_entry wait = { current, NULL };
    unsigned long flags;

    // the queue cannot be null
    if (!queue) {
        panic("tm: sleep on a null wait queue.\n");
    }

    // cannot sleep the idle task
    if (current == task[0]) {
        panic("tm: trying to sleep idle task.\n");
    }

    // switch the task in interruptible state
    current->state = TASK_INTERRUPTIBLE;

    // add task in wait queue
    add_wait_queue(queue, &wait);

    // save EFLAGS and enable interrupt if disabled
    save_flags(flags); sti();

    // schedule another task
    schedule();

    // ok, this task is running now, remove it from wait queue and restore its
    // EFLAGS register
    del_wait_queue(queue, &wait);

    restore_flags(flags);
}
