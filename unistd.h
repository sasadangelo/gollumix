/*
 * unistd.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef UNISTD_H
#define UNISTD_H

// the syscall index in the syscall table.
#define __NR_print  0
#define __NR_fork   1
#define __NR_exec   2

#define _syscall0(type,name) \
extern inline type name(void) \
{ \
type __res; \
__asm__ volatile ("int $0x80" \
    : "=a" (__res) \
    : "0" (__NR_##name)); \
    if (__res >= 0) \
        return __res; \
    errno = -__res; \
    return -1; \
}

#define _syscall1(type,name,atype,a) \
extern inline type name(atype a) \
{ \
type __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
    : "0" (__NR_##name),"b" (a)); \
    if (__res >= 0) \
        return __res; \
    errno = -__res; \
    return -1; \
}

extern int errno;

// the signature of the supported system call
int print(char *msg);
int fork(void);
int exec(char *filename);

#endif
