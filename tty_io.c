/*
 * tty_io.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "tty.h"

/* This routine initializes a TTY layer. The TTY layer could be composed by
 * several terminal where, each one can have console or serial lines
 */
void tty_init(void) {
    // initializes the console
    con_init();
}
