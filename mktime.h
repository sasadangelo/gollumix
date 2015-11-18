/*
 * mktime.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef LINUX_MKTIME_H
#define LINUX_MKTIME_H

struct mktime {
    int sec;
    int min;
    int hour;
    int day;
    int mon;
    int year;
};

#endif

