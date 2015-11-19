#ifndef SERIAL_H
#define SERIAL_H

struct serial_struct {
    short type;
    unsigned int minor;
    short base;
    short irq;
    int   flags;
    int   speed;
    void (*isr)(void);
};

// serial routines
extern void rs_init(void);

#endif
