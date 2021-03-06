/*
 * tty.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <asm/system.h>
#include <gollumix/serial.h>
#include <gollumix/console.h>
#include <gollumix/fs.h>
#include <gollumix/sched.h>
#include <gollumix/kernel.h>
#include <gollumix/stddef.h>
#include <asm/uaccess.h>

#define N_TTYS          N_CONSOLES+N_SERIALS
#define IS_VALID_TTY(n) ((n) >= 1 && (n) <= N_TTYS)

#define INIT_QUEUE { { NULL }, 0, 0, 0, ""}

// TTY_TABLE
//
// /dev/tty0    tty_table[1]    dev = 1
// /dev/tty1    tty_table[2]    dev = 2
// /dev/tty2    tty_table[3]    dev = 3
// /dev/tty3    tty_table[4]    dev = 4
// /dev/tty4    tty_table[5]    dev = 5
// /dev/tty5    tty_table[6]    dev = 6
// /dev/ttyS0   tty_table[7]    dev = 7
// /dev/ttyS1   tty_table[8]    dev = 8
#define TTY_TABLE(dev) &tty_table[dev];

struct tty_struct tty_table[N_TTYS+1] = {
    { 0, 0, INIT_QUEUE, INIT_QUEUE, INIT_QUEUE, con_write },  // not used
    { 0, 0, INIT_QUEUE, INIT_QUEUE, INIT_QUEUE, con_write },  // tty0 
    { 0, 0, INIT_QUEUE, INIT_QUEUE, INIT_QUEUE, con_write },  // tty1
    { 0, 0, INIT_QUEUE, INIT_QUEUE, INIT_QUEUE, con_write },  // tty2
    { 0, 0, INIT_QUEUE, INIT_QUEUE, INIT_QUEUE, con_write },  // tty3
    { 0, 0, INIT_QUEUE, INIT_QUEUE, INIT_QUEUE, con_write },  // tty4
    { 0, 0, INIT_QUEUE, INIT_QUEUE, INIT_QUEUE, con_write },  // tty5
    { 0, 0, INIT_QUEUE, INIT_QUEUE, INIT_QUEUE, rs_write  },  // ttyS0
    { 0, 0, INIT_QUEUE, INIT_QUEUE, INIT_QUEUE, rs_write  },  // ttyS1
};

struct tty_struct *ctty = &tty_table[1];
static struct file_operations tty_fops;

static void copy_to_cooked(struct tty_struct *tty);

//-----------------------------------------------------------------------------
// PUBLIC functions
//-----------------------------------------------------------------------------

// This routine initializes a TTY layer. The TTY layer could be composed by
// several terminal where, each one can have console or serial lines
void tty_init(void) {
    chrdev_fops[0] = &tty_fops;
    // initializes the console and serial line
    con_init();
    rs_init();
}

// tty switch
void tty_switch(int n) {
    long flags;

    if (!IS_VALID_TTY(n) || ctty == &tty_table[n]) {
        return;
    }

    save_flags(flags); cli();

    ctty = &tty_table[n];
    con_switch(n);

    restore_flags(flags);
}

//-----------------------------------------------------------------------------
// PRIVATE functions
//-----------------------------------------------------------------------------

// open a specific tty. Basically it means associate to a process a specific
// tty through its tty integer field.
//
// current->tty = <the tty specified by the user process>
//
// filp->dev is the index in tty[]
static int tty_open(struct file * filp) {
    struct tty_struct *tty;

    tty = TTY_TABLE(filp->dev);
    tty->count++;
    current->tty = filp->dev;

    // TODO
    // if it is a serial line call serial_open. This should activate the
    // interrupts on the serial line.
    return 0;
}

// close a tty
static void tty_close(struct file * filp) {
    struct tty_struct * tty;

    tty = TTY_TABLE(filp->dev);
    current->tty=-1;

    if (--tty->count) {
        return;
    }
}

static int read_chan(struct file *file, char *buf, int nr) { 
    struct tty_struct *tty;
    char *b = buf;
    int c;

    tty = TTY_TABLE(file->dev);

    TTY_READ_FLUSH(tty);

    while (nr > 0) {
        cli();
        // check if other data can be put on read_q
        if (EMPTY(&tty->secondary) && !FULL(&tty->read_q)) {
            // no data available for processes, wait!!!
            sleep(&tty->read_q.wait);
            sti();
            TTY_READ_FLUSH(tty);
            continue;
        }

        // ok, data are available for processes
        sti();
        do {
            // read nr data, if possible.
            c = getch(&tty->secondary);
            put_user_byte(c, b++);
            --nr;
        } while (nr > 0 && !EMPTY(&tty->secondary));

        wake_up(&tty->read_q.wait);
    }

    sti();
    TTY_READ_FLUSH(tty);

    if (b-buf) {
        return b - buf;
    }

    return 0;
}

static int write_chan(struct file * file, char * buf, int nr) { 
    struct tty_struct * tty;
    char c, *b=buf;

    tty = TTY_TABLE(file->dev);

    while (nr > 0) {
        if (FULL(&tty->write_q)) {
            TTY_WRITE_FLUSH(tty);
            cli();
            if (FULL(&tty->write_q)) {
                sleep(&tty->write_q.wait);
            }
            sti();
            continue;
        }

        while (nr > 0 && !FULL(&tty->write_q)) {
            c = get_user_byte(b);
            b++; nr--;
            putch(c, &tty->write_q);
        }

        if (nr > 0) {
            schedule();
        }
    }

    TTY_WRITE_FLUSH(tty);

    if (b-buf) {
        return b - buf;
    }

    return 0;
}

// read data from tty[current->tty]
static int tty_read(struct file * file, char * buf, int count) {
    return read_chan(file, buf, count);
}

// write data to tty[current->tty]
static int tty_write(struct file * file, char * buf, int count) {
    return write_chan(file, buf, count);
}

static struct file_operations tty_fops = {
    tty_open,
    tty_read,
    tty_write,
    tty_close
};

static void copy_to_cooked(struct tty_struct *tty) {
    int c;

    while (!EMPTY(&tty->read_q) && !FULL(&tty->secondary)) {
        c = getch(&tty->read_q);
        putch(c, &tty->secondary);
    }

    wake_up(&tty->secondary.wait);
}
