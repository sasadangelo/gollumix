/*
 * unistd.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef UNISTD_H
#define UNISTD_H

#include <gollumix/types.h>
#include <gollumix/stddef.h>

// the syscall index in the syscall table.
#define __NR_print  0
#define __NR_fork   1
#define __NR_exec   2
#define __NR_open   3
#define __NR_close  4
#define __NR_read   5
#define __NR_write  6

#define _syscall0(type,name) \
type name(void) \
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
type name(atype a) \
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

#define _syscall3(type,name,atype,a,btype,b,ctype,c) \
type name(atype a,btype b,ctype c) \
{ \
type __res; \
__asm__ volatile ("int $0x80" \
    : "=a" (__res) \
    : "0" (__NR_##name),"b" (a),"c" (b),"d" (c)); \
	if (__res<0) \
	    errno=-__res , __res = -1; \
	return __res;\
}

extern int errno;

// the signature of the supported system call
extern int print(char *msg);
extern int fork(void);
extern int exec(char *filename);
extern int open(const char *pathname);
extern int close(int fd);
extern ssize_t read (int fd, char *buf, size_t count);
extern ssize_t write(int fd, char *buf, size_t count);

#endif
