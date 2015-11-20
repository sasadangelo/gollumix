/*
 * tty_queue.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#ifndef TTY_QUEUE_H
#define TTY_QUEUE_H

#include <gollumix/types.h>
#include <gollumix/wait_queue.h>

#define TTY_BUF_SIZE 2048

#define EMPTY(a) ((a)->head == (a)->tail)
#define LEFT(a) (((a)->tail-(a)->head-1)&(TTY_BUF_SIZE-1))
#define FULL(a) (!LEFT(a))

#define INC(a) ((a) = ((a)+1) & (TTY_BUF_SIZE-1))

struct tty_queue {
    struct wait_queue wait;
    unsigned long data;
    unsigned long head;
    unsigned long tail;
    char buf[TTY_BUF_SIZE];
};

extern void put_queue (struct tty_queue* queue, int ch);
extern void puts_queue(struct tty_queue* queue, char *cp);
extern inline void putch(char c, struct tty_queue * queue);
extern inline char getch(struct tty_queue * queue);

#endif
