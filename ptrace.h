/*
 * ptrace.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef _PTRACE_H
#define _PTRACE_H

/* this struct defines the way the registers are stored on the
 * stack during a system call. */

struct pt_regs {
    long ebx;
    long ecx;
    long edx;
    long esi;
    long edi;
    long ebp;
    long eax;
    int  xds;
    int  xes;
    long eip;
    int  xcs;
    long eflags;
    long esp;
    int  xss;
};

#endif
