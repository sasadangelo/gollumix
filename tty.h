/*
 * tty.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef TTY_H
#define TTY_H

struct tty_struct {
    int (*write)(char *buffer, int size);
};

// tty routines
extern void tty_init(void);
//extern int  get_current_tty_index(void);
extern void tty_switch(int n);

// current tty
extern struct tty_struct *ctty;

#endif
