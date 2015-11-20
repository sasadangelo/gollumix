#include "system.h"
#include "kernel.h"
#include "stddef.h"
#include "wait_queue.h"

void add_wait_queue(struct wait_queue *queue, struct wait_entry *entry) {
    unsigned long flags;

    save_flags(flags); cli();

    if (queue) {

        if (!queue->head) {
            entry->next = entry;
            queue->head = entry;
        } else {
            entry->next = queue->head->next;
            queue->head->next = entry;
        }

    } else {
        panic("tm: null wait queue.\n");
    }

    restore_flags(flags);
}

void del_wait_queue(struct wait_queue *queue, struct wait_entry *entry) {
    struct wait_entry *tmp;
    unsigned long flags;

    save_flags(flags); cli();

    if (queue) {
        if (queue->head == entry && ((queue->head = entry->next) == entry)) {
            queue->head = NULL;
        } else {
            tmp = entry;

            while (tmp->next != entry) {
                tmp = tmp->next;
            }

            tmp->next = entry->next;
        }
        entry->next = NULL;
    } else {
        panic("tm: null wait queue.\n");
    }

    restore_flags(flags);
}
