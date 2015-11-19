/*
 * tty_io.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include "system.h"
#include "tty.h"
#include "serial.h"
#include "console.h"

#define N_TTYS          N_CONSOLES
#define IS_VALID_TTY(n) ((n) >= 1 && (n) <= N_TTYS)

struct tty_struct tty[N_TTYS+1] = {
    { con_write },  // not used
    { con_write },  // tty 1
    { con_write },  // tty 2
    { con_write },  // tty 3
    { con_write },  // tty 4
    { rs_write  },  // tty 5
};

struct tty_struct *ctty = &tty[1];

// This routine initializes a TTY layer. The TTY layer could be composed by
// several terminal where, each one can have console or serial lines
void tty_init(void) {
    // initializes the console and serial line
    con_init();
    rs_init();
}

/*
// return index of current tty
int get_current_tty_index(void) {
    return ctty - tty;
}
*/

// tty switch
void tty_switch(int n) {
    long flags;

    if (IS_VALID_TTY(n) && ctty != &tty[n]) {
        save_flags(flags); cli();

        ctty = &tty[n];
        con_switch(n);

        restore_flags(flags);
    }
}

