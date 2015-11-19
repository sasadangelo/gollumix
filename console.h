#ifndef CONSOLE_H
#define CONSOLE_H

#define N_CONSOLES          5

struct console_struct {
    unsigned long video;    // start video RAM for console n
    unsigned long pos;      // absolute position in video RAM
    unsigned long x,y;
    unsigned char attr;
};

// console routines
extern void con_init  (void);
extern int  con_write (char *buffer, int size);
extern int  con_write2(int n, char *buffer, int size);
extern void con_switch(int n);

#endif
