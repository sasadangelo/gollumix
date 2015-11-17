#include "io.h"
#include "system.h"
#include "kernel.h"
#include "keymap.h"

typedef void (*fptr)(int);

static fptr key_table[];

extern void kb_intr(void);

/*
 * the keyboard interrupt handler. Here it prints a simple "Hello" message.
 */
void keyboard_interrupt(void) {
	unsigned char scancode, x;

    scancode=inb_p(0x60);
    x=inb_p(0x61);
    outb_p(x|0x80, 0x61);
    outb_p(x&0x7f, 0x61);
    outb(0x20, 0x20);

	key_table[scancode](scancode);
}

/*
 * Initialize the keyboard driver associating to it IRQ1 (interrupt 0x21)
 */
void keyboard_init(void) {
    register unsigned char a;

    // programming the PIC 8259 Master Controller in order to associate IRQ1
    // to the keyboard interrupt handler. Remember the interrupt
    // vector of IRQ1 is 0x21 (see PIC programming in setup.S).
    set_intr_gate(0x21, &kb_intr);
    outb_p(inb_p(0x21) & 0xfd, 0x21);

	// disable/enable the keyboard
    a=inb_p(0x61);
    outb_p(a|0x80, 0x61);
    outb(a, 0x61);
}

static void none(int sc) {
}

static void do_self(int sc) {
	unsigned char ch;

	ch=key_map[sc];

	if (ch == 0) {
		return;
	}

	printk("%c", ch);
}

static void enter(int sc) {
}

static void ctrl(int sc) {
}

static void unctrl(int sc) {
}

static void lshift(int sc) {
}

static void unlshift(int sc) {
}

static void slash(int sc) {
}

static void rshift(int sc) {
}

static void unrshift(int sc) {
}

static void star(int sc) {
}

static void alt(int sc) {
}

static void unalt(int sc) {
}

static void caps(int sc) {
}

static void uncaps(int sc) {
}

static void func(int sc) {
}

static void num(int sc) {
}

static void scroll(int sc) {
}

static void cursor(int sc) {
}

static void minus(int sc) {
}

static void plus(int sc) {
}

static fptr key_table[] = {
    none,do_self,do_self,do_self,       /* 00-03 s0 esc 1 2 */
    do_self,do_self,do_self,do_self,    /* 04-07 3 4 5 6 */
    do_self,do_self,do_self,do_self,    /* 08-0B 7 8 9 0 */
    do_self,do_self,do_self,do_self,    /* 0C-0F + ' bs tab */
    do_self,do_self,do_self,do_self,    /* 10-13 q w e r */
    do_self,do_self,do_self,do_self,    /* 14-17 t y u i */
    do_self,do_self,do_self,do_self,    /* 18-1B o p } ^ */
    enter,ctrl,do_self,do_self,         /* 1C-1F enter ctrl a s */
    do_self,do_self,do_self,do_self,    /* 20-23 d f g h */
    do_self,do_self,do_self,do_self,    /* 24-27 j k l | */
    do_self,do_self,lshift,do_self,     /* 28-2B { para lshift , */
    do_self,do_self,do_self,do_self,    /* 2C-2F z x c v */
    do_self,do_self,do_self,do_self,    /* 30-33 b n m , */
    do_self,slash,rshift,star,          /* 34-37 . - rshift * */
    alt,do_self,caps,func,              /* 38-3B alt sp caps f1 */
    func,func,func,func,                /* 3C-3F f2 f3 f4 f5 */
    func,func,func,func,                /* 40-43 f6 f7 f8 f9 */
    func,num,scroll,cursor,             /* 44-47 f10 num scr home */
    cursor,cursor,minus,cursor,         /* 48-4B up pgup - left */
    cursor,cursor,plus,cursor,          /* 4C-4F n5 right + end */
    cursor,cursor,cursor,cursor,        /* 50-53 dn pgdn ins del */
    none,none,do_self,func,             /* 54-57 sysreq ? < f11 */
    func,none,none,none,                /* 58-5B f12 ? ? ? */
    none,none,none,none,                /* 5C-5F ? ? ? ? */
    none,none,none,none,                /* 60-63 ? ? ? ? */
    none,none,none,none,                /* 64-67 ? ? ? ? */
    none,none,none,none,                /* 68-6B ? ? ? ? */
    none,none,none,none,                /* 6C-6F ? ? ? ? */
    none,none,none,none,                /* 70-73 ? ? ? ? */
    none,none,none,none,                /* 74-77 ? ? ? ? */
    none,none,none,none,                /* 78-7B ? ? ? ? */
    none,none,none,none,                /* 7C-7F ? ? ? ? */
    none,none,none,none,                /* 80-83 ? br br br */
    none,none,none,none,                /* 84-87 br br br br */
    none,none,none,none,                /* 88-8B br br br br */
    none,none,none,none,                /* 8C-8F br br br br */
    none,none,none,none,                /* 90-93 br br br br */
    none,none,none,none,                /* 94-97 br br br br */
    none,none,none,none,                /* 98-9B br br br br */
    none,unctrl,none,none,              /* 9C-9F br unctrl br br */
    none,none,none,none,                /* A0-A3 br br br br */
    none,none,none,none,                /* A4-A7 br br br br */
    none,none,unlshift,none,            /* A8-AB br br unlshift br */
    none,none,none,none,                /* AC-AF br br br br */
    none,none,none,none,                /* B0-B3 br br br br */
    none,none,unrshift,none,         /* B4-B7 br br unrshift br */
    unalt,none,uncaps,none,             /* B8-BB unalt br uncaps br */
    none,none,none,none,                /* BC-BF br br br br */
    none,none,none,none,                /* C0-C3 br br br br */
    none,none,none,none,                /* C4-C7 br br br br */
    none,none,none,none,                /* C8-CB br br br br */
    none,none,none,none,                /* CC-CF br br br br */
    none,none,none,none,                /* D0-D3 br br br br */
    none,none,none,none,                /* D4-D7 br br br br */
    none,none,none,none,                /* D8-DB br ? ? ? */
    none,none,none,none,                /* DC-DF ? ? ? ? */
    none,none,none,none,                /* E0-E3 e0 e1 ? ? */
    none,none,none,none,                /* E4-E7 ? ? ? ? */
    none,none,none,none,                /* E8-EB ? ? ? ? */
    none,none,none,none,                /* EC-EF ? ? ? ? */
    none,none,none,none,                /* F0-F3 ? ? ? ? */
    none,none,none,none,                /* F4-F7 ? ? ? ? */
    none,none,none,none,                /* F8-FB ? ? ? ? */
    none,none,none,none                 /* FC-FF ? ? ? ? */
};

