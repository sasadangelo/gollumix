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

#define N_CONSOLES   4
#define MEM_VIDEO   0xb8000
#define SCR_W       80
#define SCR_H       25
#define SCR_SIZE    (SCR_W*SCR_H*2)
#define MAX_USER_MSG 255
#define is_valid_console(n) ((n)>=1 && (n)<=N_CONSOLES)

struct console_s {
    unsigned long video;    // start video RAM for console n
    unsigned long pos;      // absolute position in video RAM
    unsigned long x,y;
    unsigned char attr;
};

#define MAKE_CONSOLE(n) { \
    MEM_VIDEO + SCR_SIZE*(n-1), \
    MEM_VIDEO + SCR_SIZE*(n-1), \
    0,0,0x07}

static struct console_s console[N_CONSOLES+1] = {
    MAKE_CONSOLE(1),    // Not used
    MAKE_CONSOLE(1),    // Console 1
    MAKE_CONSOLE(2),    // Console 2
    MAKE_CONSOLE(3),    // Console 3
    MAKE_CONSOLE(4),    // Console 4
};

/* Current console: */
static struct console_s *cc = &console[1];

#define VIDEO (cc->video)
#define X     (cc->x)
#define Y     (cc->y)
#define POS   (cc->pos)
#define ATTR  (cc->attr)

typedef struct {
    short data[SCR_W];
} LINE;

extern void keyboard_init(void);

static void scroll_screen(void);

/*
 * This routine move the cursor on the position (new_x, new_y).
 */
static inline void gotoxy(unsigned int new_x,unsigned int new_y) {
    if (new_x>=SCR_W || new_y>=SCR_H)
        return;
    X=new_x;
    Y=new_y;
    POS=VIDEO+((Y*SCR_W+X)<<1);
}

/*
 * This function initializes the console driver. A console is composed by a
 * keyboard and video.
 */
void con_init(void) {
    // the memory location 0x94010 contains the current cursor position.
    // Please see setup.S.
    unsigned char y = *(unsigned char *)(0x94011);

    // if the boot process has printed the last line at the bottom of the
    // screen, it is scrolled and the printing continue to the last line.
    if (++y >= SCR_H) {
        scroll_screen();
        y = SCR_H-1;
    }

    gotoxy(0, y);

    // initialize the keyboard driver
    keyboard_init();
}

static void clear_line(int n) {
    char *mem;
    char *line_end;

    if (n<0 && n>SCR_H-1) {
        return;
    }

    mem = (char*)VIDEO + (SCR_W*2)*n;
    line_end = mem + (SCR_W*2);

    for (; mem<line_end; mem+=2) {
        *mem = 0x20;        /* 0x20 = ' ' */
        *(mem+1) = ATTR;
    }
}

static void scroll_screen(void) {
    LINE *memvideo = (LINE*)VIDEO;

    // Move the rows upwards  */
    memmove(memvideo, &memvideo[1], sizeof(LINE)*(SCR_H-1));

    // clear last line
    clear_line(SCR_H-1);
}

static inline void set_6845(int reg, int val) {
    const int vid_port = 0x3D4;

    outb_p (reg,    vid_port);  // low byte
    outb_p (val>>8, vid_port + 1);

    outb_p (reg+1,  vid_port);  // high byte
    outb_p (val,    vid_port + 1);
}

static inline void set_cursor_pos(void) {
    set_6845(14, (VIDEO-MEM_VIDEO)/2 + Y*SCR_W+X);
}

static inline void print_char(char c) {
    switch (c) {
    case '\n':
        Y++;
        X=0;
        break;
    case '\t':
        X += 8;
        break;
    case '\a':
        //beep(750,20);
        break;
    default:
        *(char*)POS = c;
        *(char*)(POS+1) = ATTR;
        X++;
    }

    if (X>=SCR_W) {
        Y += X/SCR_W;  // new line
        X = X%SCR_W;
    }
    if (Y>=SCR_H) {
        scroll_screen();
        Y = SCR_H-1;
    }

    POS=VIDEO+((Y*SCR_W+X)<<1);
    set_cursor_pos();
}

static char buf[1024];

asmlinkage int printk(const char *fmt, ...) {
    va_list args;
    int len;
    char *ch;
    long flags;

    va_start(args, fmt);
    len = vsprintf(buf,fmt,args);
    va_end(args);

    save_flags(flags); cli();

    for (ch=buf; *ch; ch++)
        print_char (*ch);

    restore_flags(flags);
    return len;
}

static inline void set_origin(void) {
    outb_p(12,0x3d4);
    outb_p(0xff&((VIDEO-MEM_VIDEO)>>9),0x3d5);
    outb_p(13,0x3d4);
    outb_p(0xff&((VIDEO-MEM_VIDEO)>>1),0x3d5);
}

void switch_to_console(int n) {
    long flags;

    if (is_valid_console(n) && cc != &console[n]) {
        save_flags(flags); cli();

        cc = &console[n];
        set_origin();
        set_cursor_pos();

        restore_flags(flags);
    }
}

/*
 * A simple implementation of a debug system call
 */
asmlinkage int sys_print(char *msg) {
    char str[MAX_USER_MSG+1];

    strncpy_from_user(str, msg, sizeof(str));
	printk("%s", str);
    return 0;
}

