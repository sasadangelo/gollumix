#ifndef SERIAL_H
#define SERIAL_H

#include "tty.h"

#define RS_QUEUE_MAX    4096
#define N_SERIALS       2

struct serial_struct {
    short type;
    unsigned int minor;
    short base;
    short irq;
    int   flags;
    int   speed;
    void (*isr)(void);
};

// serial routines
extern void rs_init(void);
extern void rs_write(struct tty_struct *tty);

#endif
