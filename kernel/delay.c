#include <gollumix/delay.h>
#include <gollumix/kernel.h>
#include <gollumix/sched.h>

extern volatile unsigned long jiffies;

// this should be approx 2 Bo*oMips to start (note initial shift), and will
// still work even if initially too large, it will just take slightly longer
//
// loops_per_jiffy = 0x00001000
static unsigned long loops_per_jiffy = 1<<12;

// This is the number of bits of precision for the loops_per_jiffy.  Each
// bit takes on average 1.5/HZ seconds.  This (like the original) is a little
// better than 1%
#define LPS_PREC 8

static inline void __loop_delay(unsigned long loops) {
    int d0;

    __asm__ __volatile__(
        "       jmp 1f  \n"
        ".align 16      \n"
        "1:     jmp 2f  \n"
        ".align 16      \n"
        "2:     decl %0 \n"
        "       jns 2b  \n"
        : "=&a" (d0)
        : "0"   (loops));
}


static void __delay(unsigned long loops) {
    __loop_delay(loops);
}

static inline void __const_udelay(unsigned long xloops) {
    int d0;

    __asm__("mull %0"
        :"=d" (xloops), "=&a" (d0)
        :"1" (xloops),"0" (loops_per_jiffy));
    __delay(xloops * HZ);
}

void udelay(unsigned long usecs) {
    __const_udelay(usecs * 0x000010c6);
}

void mdelay(unsigned long msec) {
    while (msec--) udelay(1000);
}

// the goal of this routine is to find the equivalence in term of time between
// a jiffies and a num o loops. This is platform dependant, so it must be
// calibrate. This calibration is useful since I can translate a request like
//
//			delay(num jiffies)
//
// in
//
//          delay(num loops)
void calibrate_delay(void) {
    unsigned long ticks, loopbit;
    int lps_precision = LPS_PREC;

    // set an initial value for loops_per_jiffy = 0x00001000
    // loops_per_jiffy = 1<<12;

    printk("Calibrating delay loop ... ");

    // This first loop is necessary to find a number of loops such that:
    // 
    //                TIME(loops_per_jiffy) >= TIME(1 jiffies)
    //
    // This while loop try th following values for loops_per_jiffy:
    //
    // 0x00001000, 0x00002000, 0x00004000, 0x00008000, 0x00010000, etc.
    while (loops_per_jiffy <<= 1) {
        // wait for "start of" clock tick
        ticks = jiffies;
        while (ticks == jiffies);
        // Go ..
        ticks = jiffies;
        // I sleep for a number of loop equal to loops_per_jiffy. If 1 jiffies
        // does not pass I increase the loops_per_jiffy and retry again.
        __delay(loops_per_jiffy);
        ticks = jiffies - ticks;

        if (ticks) {
            // ok, at least 1 jiffies is passed
            break;
        }
    }

    // Ok, now TIME(number of loops) >= TIME(1 jiffies)
    // Suppose loops_per_jiffy == 0x00004000, the next loop now try to
    // find the correct value of loops_per_jiffy in the range:
    // [0x00002000 - 0x00004000].
    loops_per_jiffy >>= 1;
    loopbit = loops_per_jiffy;
    while ( lps_precision-- && (loopbit >>= 1) ) {
        loops_per_jiffy |= loopbit;
        ticks = jiffies;
        while (ticks == jiffies);
        ticks = jiffies;
        __delay(loops_per_jiffy);
        if (jiffies != ticks)
            loops_per_jiffy &= ~loopbit;
    }

    // Round the value and print it */
    printk("%lu.%02lu BogoMIPS\n", loops_per_jiffy/(500000/HZ),
                                   (loops_per_jiffy/(5000/HZ)) % 100);
}

