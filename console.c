#include "system.h"
#include "vsprintf.h"
#include "io.h"
#include "kernel.h"
#include "string.h"

#define MEM_VIDEO   0xb8000
#define SCR_W       80
#define SCR_H       25

struct console_s {
    unsigned long video;    // start video RAM for console n
    unsigned long pos;      // absolute position in video RAM
    unsigned long x,y;
    unsigned char attr;
};

/* Current console: */
static struct console_s cc = { MEM_VIDEO, MEM_VIDEO, 0, 0, 0x07};

#define VIDEO (cc.video)
#define X     (cc.x)
#define Y     (cc.y)
#define POS   (cc.pos)
#define ATTR  (cc.attr)

typedef struct {
    short data[80];
} LINE;

extern void keyboard_init(void);

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
    // goto (0, 0) on the screen
    gotoxy(0, 0);

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

static char buf[1024];

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
																			//
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
