/*
 * tty.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#ifndef TTY_H
#define TTY_H

#include "tty_queue.h"

struct tty_struct {
    // reference count
    int count;

    // tty queues
    struct tty_queue read_q;           // the data coming from keyboard or
                                       // serial ISR
    struct tty_queue write_q;          // the data sent to console or serial
                                       // line
    struct tty_queue secondary;        // when read_q is full the data in it
                                       // will be transfered here. The processes
                                       // read data from this queue.

    // write method
    int (*write)(char *buffer, int size);
};

// tty routines
extern void tty_init(void);
extern void tty_switch(int n);

// current tty
extern struct tty_struct *ctty;

#endif
