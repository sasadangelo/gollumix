#ifndef DELAY_H
#define DELAY_H

extern void udelay(unsigned long usecs);
extern void mdelay(unsigned long msec);
extern void calibrate_delay(void);

#endif
