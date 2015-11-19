#ifndef SERIAL_H
#define SERIAL_H

#define RS_QUEUE_MAX    4096

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
extern int  rs_write(char *buffer, int size);

#endif
