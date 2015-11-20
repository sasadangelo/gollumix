/*
 * mc146818rtc.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef MC146818RTC_H
#define MC146818RTC_H

#include <asm/io.h>

#define CMOS_READ(addr) ({ \
    outb_p(addr|0x80,0x70); \
    inb_p(0x71); \
    })

#define RTC_SECONDS         0
#define RTC_SECONDS_ALARM   1
#define RTC_MINUTES         2
#define RTC_MINUTES_ALARM   3
#define RTC_HOURS           4
#define RTC_HOURS_ALARM     5

/* RTC_*_alarm is always true if 2 MSBs are set */
# define RTC_ALARM_DONT_CARE    0xC0

#define RTC_DAY_OF_WEEK     6
#define RTC_DAY_OF_MONTH    7
#define RTC_MONTH           8
#define RTC_YEAR            9

/* control registers - Moto names
 *  */
#define RTC_REG_A       10
#define RTC_REG_B       11
#define RTC_REG_C       12
#define RTC_REG_D       13

#define RTC_FREQ_SELECT RTC_REG_A

# define RTC_UIP        0x80

#define RTC_CONTROL RTC_REG_B
#define RTC_DM_BINARY 0x04 /* all time/date values are BCD if clear */

#ifndef BCD_TO_BIN
#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)
#endif

#ifndef BIN_TO_BCD
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)
#endif

#endif 

