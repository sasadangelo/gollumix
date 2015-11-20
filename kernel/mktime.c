/*
 * mktime.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "mktime.h"

#define MINUTE 60
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)
#define YEAR (365*DAY)

static int month[12] = {
    0,
    DAY*(31),
    DAY*(31+29),
    DAY*(31+29+31),
    DAY*(31+29+31+30),
    DAY*(31+29+31+30+31),
    DAY*(31+29+31+30+31+30),
    DAY*(31+29+31+30+31+30+31),
    DAY*(31+29+31+30+31+30+31+31),
    DAY*(31+29+31+30+31+30+31+31+30),
    DAY*(31+29+31+30+31+30+31+31+30+31),
    DAY*(31+29+31+30+31+30+31+31+30+31+30)
};

long kernel_mktime(struct mktime * time) {
    long res;
    int year;

    year = time->year - 70;
    // magic offsets (y+1) needed to get leapyears right.
    res = YEAR*year + DAY*((year+1)/4);
    res += month[time->mon];
    // and (y+2) here. If it wasn't a leap-year, we have to adjust
    if (time->mon>1 && ((year+2)%4))
        res -= DAY;
    res += DAY*(time->day-1);
    res += HOUR*time->hour;
    res += MINUTE*time->min;
    res += time->sec;
    return res;
}

