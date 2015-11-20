#ifndef CONSOLE_H
#define CONSOLE_H

#include "tty.h"

#define N_CONSOLES          6

struct console_struct {
    unsigned long video;    // start video RAM for console n
    unsigned long pos;      // absolute position in video RAM
    unsigned long x,y;
    unsigned char attr;
};

// console routines
extern void con_init  (void);
extern void con_write (struct tty_struct *tty);
extern void con_switch(int n);

#endif
