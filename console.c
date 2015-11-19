/*
 * console.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "system.h"
#include "vsprintf.h"
#include "io.h"
#include "kernel.h"
#include "string.h"
#include "uaccess.h"
#include "console.h"

#define ORIG_X 0
// the memory location 0x94010 contains the current cursor position.
// Please see setup.S.
#define ORIG_Y *(unsigned char *)(0x94011)

#define MEM_VIDEO   0xb8000
#define SCR_W       80
#define SCR_H       25
#define SCR_SIZE    (SCR_W*SCR_H*2)
#define MAX_USER_MSG 255
#define IS_VALID_CONSOLE(n) ((n)>=1 && (n)<=N_CONSOLES)

#define MAKE_CONSOLE(n) { \
    MEM_VIDEO + SCR_SIZE*(n-1), \
    MEM_VIDEO + SCR_SIZE*(n-1), \
    0,0,0x07}

static struct console_struct console[N_CONSOLES+1] = {
    MAKE_CONSOLE(1),    // Not used
    MAKE_CONSOLE(1),    // Console 1
    MAKE_CONSOLE(2),    // Console 2
    MAKE_CONSOLE(3),    // Console 3
    MAKE_CONSOLE(4),    // Console 4
    MAKE_CONSOLE(5),    // Console 5
};

/* Current console: */
static struct console_struct *cc = &console[1];

typedef struct {
    short data[SCR_W];
} LINE;

extern void keyboard_init(void);

static void scroll_screen(struct console_struct *con);

// This routine move the cursor on the position (new_x, new_y).
static inline void gotoxy(struct console_struct *con,
                          unsigned int new_x, 
						  unsigned int new_y) {
    if (new_x>=SCR_W || new_y>=SCR_H)
        return;
    con->x=new_x;
    con->y=new_y;
    con->pos=con->video+((con->y*SCR_W+con->x) << 1);
}

// This function initializes the console driver. A console is composed by a
// keyboard and video.
void con_init(void) {
    unsigned char y = ORIG_Y;

    // if the boot process has printed the last line at the bottom of the
    // screen, it is scrolled and the printing continue to the last line.
    if (++y >= SCR_H) {
        scroll_screen(&console[1]);
        y = SCR_H-1;
    }

    gotoxy(&console[1], ORIG_X, y);

    // initialize the keyboard driver
    keyboard_init();
}

// this routine clear the n-th line
static void clear_line(struct console_struct *con, int n) {
    char *mem;
    char *line_end;

    if (n<0 && n>SCR_H-1) {
        return;
    }

    mem = (char*)con->video + (SCR_W*2)*n;
    line_end = mem + (SCR_W*2);

    for (; mem<line_end; mem+=2) {
        *mem = 0x20;        /* 0x20 = ' ' */
        *(mem+1) = con->attr;
    }
}

// this routine scroll the current screen by a line
static void scroll_screen(struct console_struct *con) {
    LINE *memvideo = (LINE*)con->video;

    // Move the rows upwards  */
    memmove(memvideo, &memvideo[1], sizeof(LINE)*(SCR_H-1));

    // clear last line
    clear_line(con, SCR_H-1);
}

// help routine used to move the cursor
static inline void set_6845(int reg, int val) {
    const int vid_port = 0x3D4;

    outb_p (reg,    vid_port);  // low byte
    outb_p (val>>8, vid_port + 1);

    outb_p (reg+1,  vid_port);  // high byte
    outb_p (val,    vid_port + 1);
}

// move the cursor of the current screen n the position defined by X and Y
static inline void set_cursor_pos(struct console_struct *con) {
    set_6845(14, (con->video - MEM_VIDEO)/2 + con->y*SCR_W+con->x);
}

// print a character on the current screen at the current cursor position.
// This routine handle also the cursor movement.
static inline void print_char(struct console_struct *con, char c) {
    switch (c) {
    case '\n':
        con->y++;
        con->x=0;
        break;
    case '\t':
        con->x += 8;
        break;
    case '\a':
        //beep(750,20);
        break;
    default:
        *(char*)con->pos = c;
        *(char*)(con->pos+1) = con->attr;
        con->x++;
    }

    if (con->x >= SCR_W) {
        con->y += con->x/SCR_W;  // new line
        con->x = con->x % SCR_W;
    }
    if (con->y>=SCR_H) {
        scroll_screen(con);
        con->y = SCR_H-1;
    }

    con->pos=con->video+ ((con->y*SCR_W+con->x) << 1);
    set_cursor_pos(con);
}

static char buf[1024];

// write a string on console n
int con_write2(int n, char *buffer, int size) {
    long flags;
    int  i = 0;

    save_flags(flags); cli();

    while(i < size) {
        print_char(&console[n], *(buffer+i));
        i++;
    }

    restore_flags(flags);
    return size;
}

// write a string on current console
int con_write(char *buffer, int size) {
    return con_write2(cc-console, buffer, size);
}

// this is a kernel routine equivalent to the stdlib printf
asmlinkage int printk(const char *fmt, ...) {
    va_list args;
    int len;

    va_start(args, fmt);
    len = vsprintf(buf,fmt,args);
    va_end(args);

    con_write(buf, len);
    return len;
}

static inline void set_origin(struct console_struct *con) {
    outb_p(12, 0x3d4);
    outb_p(0xff & ((con->video - MEM_VIDEO) >> 9), 0x3d5);
    outb_p(13, 0x3d4);
    outb_p(0xff & ((con->video - MEM_VIDEO) >> 1), 0x3d5);
}

// console switch
void con_switch(int n) {
    long flags;

    if (IS_VALID_CONSOLE(n) && cc != &console[n]) {
        // Attention!!!
		// this routine is called by tty_switch with interrupt disabled, so
		// the instructions: save_flags(flags); cli(); ... restore_flags(flags);
		// are unecessary.
        save_flags(flags); cli();

        cc = &console[n];
        set_origin(cc);
        set_cursor_pos(cc);

        restore_flags(flags);
    }
}


// A simple implementation of a debug system call
asmlinkage int sys_print(char *msg) {
    char str[MAX_USER_MSG+1];

    strncpy_from_user(str, msg, sizeof(str));
	printk("%s", str);
    return 0;
}

