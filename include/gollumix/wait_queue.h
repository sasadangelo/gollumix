#ifndef WAIT_QUEUE_H
#define WAIT_QUEUE_H

// This is a structure of a wait queue associated to a specific resource:
//
//                   wait_entry       wait_entry       wait_entry
// Resource ----->  ------------     ------------     ------------
//                 |            |-->|            |-->|            |
//                  ------------     ------------     ------------
//                 |            |   |            |   |            |
//                  ------------     ------------     ------------
//                       |                 |                |
//                     task 1            task 2           task3
struct wait_entry {
    // the process waiting in this entry
    struct task_struct *task;
    // the next entry in the queue
    struct wait_entry  *next;
};

struct wait_queue {
    // wait queue head
    struct wait_entry *head;
};

extern void add_wait_queue(struct wait_queue *queue, struct wait_entry *entry);
extern void del_wait_queue(struct wait_queue *queue, struct wait_entry *entry);

#endif
