/*
 * tty.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#ifndef TTY_H
#define TTY_H

#include <gollumix/tty_queue.h>

#define TTY_WRITE_BUSY 1
#define TTY_READ_BUSY  2

#define TTY_WRITE_FLUSH(tty) \
do { \
    cli(); \
    if (!EMPTY(&tty->write_q) && !(TTY_WRITE_BUSY & tty->busy)) { \
        tty->busy |= TTY_WRITE_BUSY; \
        sti(); \
        tty->write(tty); \
        cli(); \
        tty->busy &= ~TTY_WRITE_BUSY; \
    } \
    sti(); \
} while (0)

#define TTY_READ_FLUSH(tty) \
do { \
    cli(); \
    if (!EMPTY(&tty->read_q) && !(TTY_READ_BUSY & tty->busy)) { \
        tty->busy |= TTY_READ_BUSY; \
        sti(); \
        copy_to_cooked(tty); \
        cli(); \
        tty->busy &= ~TTY_READ_BUSY; \
    } \
    sti(); \
} while (0)

struct tty_struct {
    // reference count
    int count;

    // indicate if someone is writing/reading to/from tty
    // This variabile could have the following values:
    // TTY_WRITE_BUSY | TTY_READ_BUSY
    int busy;

    // tty queues
    struct tty_queue read_q;           // the data coming from keyboard or
                                       // serial ISR
    struct tty_queue write_q;          // the data sent to console or serial
                                       // line
    struct tty_queue secondary;        // when read_q is full the data in it
                                       // will be transfered here. The processes
                                       // read data from this queue.

    // write method
    void (*write)(struct tty_struct *tty);
};

// tty routines
extern void tty_init(void);
extern void tty_switch(int n);

// current tty
extern struct tty_struct *ctty;
// tty table
extern struct tty_struct tty_table[];

#endif
