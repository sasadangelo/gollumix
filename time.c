/*
 * time.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "kernel.h"
#include "io.h"
#include "irq.h"
#include "sched.h"
#include "mktime.h"
#include "mc146818rtc.h"
#include "time.h"

#define RTC_ALWAYS_BCD 1

#define TIMER_COUNT ((unsigned) (TIMER_FREQ/HZ))
#define TIMER_FREQ  1193182L

#define TIMER_MODE  0x43    /* I/O port for timer mode control */
#define TIMER_0     0x40    /* timer channel 0 */
#define SQUARE_WAVE 0x36

extern struct timeval xtime;

unsigned long volatile jiffies = 0;

extern long kernel_mktime(struct mktime * time);

static void timer_interrupt(void) {
    ++jiffies;
}

static void setup_timer(void) {
    printk ("Timer:");
    outb_p (SQUARE_WAVE, TIMER_MODE);
    outb_p ((unsigned char)TIMER_COUNT, TIMER_0);
    outb_p ((unsigned char)(TIMER_COUNT>>8), TIMER_0);

    add_irq_handler (0, &timer_interrupt);
    printk (" ok!\n");
}

void time_init(void) {
	struct mktime time;
    
	do {  
        // Isn't this overkill ? UIP above should guarantee consistency 
        time.sec  = CMOS_READ(RTC_SECONDS);
        time.min  = CMOS_READ(RTC_MINUTES);
        time.hour = CMOS_READ(RTC_HOURS);
        time.day  = CMOS_READ(RTC_DAY_OF_MONTH);
        time.mon  = CMOS_READ(RTC_MONTH);
        time.year = CMOS_READ(RTC_YEAR);
    } while (time.sec != CMOS_READ(RTC_SECONDS));

	if (!(CMOS_READ(RTC_CONTROL) & RTC_DM_BINARY) || RTC_ALWAYS_BCD) {
        BCD_TO_BIN(time.sec);
        BCD_TO_BIN(time.min);
        BCD_TO_BIN(time.hour);
        BCD_TO_BIN(time.day);
        BCD_TO_BIN(time.mon);
        BCD_TO_BIN(time.year);
    }

    // print the current date and time
    printk("Date (mm/dd/yy): %02d/%02d/%02d\n",
			time.mon, 
			time.day, 
			time.year);
    printk("Time (hh:mm:ss): %02d:%02d:%02d\n",
			time.hour,
			time.min,
			time.sec);

    xtime.tv_sec = kernel_mktime(&time);

    setup_timer();
}
