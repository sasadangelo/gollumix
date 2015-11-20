/*
 * task.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef TASK_H
#define TASK_H

#include "system.h"
#include "tasks.h"
#include "fs.h"

// task states
#define TASK_RUNNING         0
//#define TASK_INTERRUPTIBLE   1

#define FIRST_TASK task[0]
#define LAST_TASK  task[NR_TASKS-1]

// task context
struct tss_struct {
    unsigned short  back_link,__blh;
    unsigned long   esp0;
    unsigned short  ss0,__ss0h;
    unsigned long   esp1;
    unsigned short  ss1,__ss1h;
    unsigned long   esp2;
    unsigned short  ss2,__ss2h;
    unsigned long   cr3;
    unsigned long   eip;
    unsigned long   eflags;
    unsigned long   eax,ecx,edx,ebx;
    unsigned long   esp;
    unsigned long   ebp;
    unsigned long   esi;
    unsigned long   edi;
    unsigned short  es, __esh;
    unsigned short  cs, __csh;
    unsigned short  ss, __ssh;
    unsigned short  ds, __dsh;
    unsigned short  fs, __fsh;
    unsigned short  gs, __gsh;
    unsigned short  ldt, __ldth;
    unsigned short  trace, bitmap;
    unsigned long   tr;
};

// This structure represents a process table entry.
struct task_struct {

    ///////////////////////////////////////////////////////////////////////
    // TASK STATE SECTION
    ///////////////////////////////////////////////////////////////////////

    // This is the process state. These are the following values:
	//
	// TASK_RUNNING
    long state;

    // the process id
    int pid;

    ///////////////////////////////////////////////////////////////////////
    // SCHEDULING SECTION
    ///////////////////////////////////////////////////////////////////////

    // dynamic priority
    long counter;
    // static priority
    long priority;

    ///////////////////////////////////////////////////////////////////////
    // TTY SECTION
    ///////////////////////////////////////////////////////////////////////

    // the tty associated to the process, -1 no tty
    int tty;

    ///////////////////////////////////////////////////////////////////////
    // MEMORY SECTION
    ///////////////////////////////////////////////////////////////////////

    // each process has only one page for code/data.
    char *mem;
    // how many "pages" the process uses 
    unsigned long used_pages;
    // local descriptor table of the process
    // 0:null
    // 1:CS
    // 2:DS,SS
    struct desc_struct ldt[3];
    // the task state segment of the process
    struct tss_struct tss;

    ///////////////////////////////////////////////////////////////////////
    // FS SECTION
    ///////////////////////////////////////////////////////////////////////
    // open file table
    struct file * filp[NR_OPEN];
};

// INIT_TASK is used to set up the first task table, touch at
// your own risk!. Base=0, limit=0x9ffff (=640kB)
#define INIT_TASK \
    {TASK_RUNNING, \
     0,  \
     15, \
     15, \
     -1, \
     0,  \
     0,  \
     {{0, 0}, \
      {0x9f, 0xc0fa00}, \
      {0x9f, 0xc0f200}, \
     }, \
     {0,0,PAGE_SIZE+(unsigned long)&init_task,0x10,0,0,0,0,0,0,0,0, \
      0,0,0,0,0,0,0,0,0,0, \
      USER_DS,0,USER_CS,0,USER_DS,0,USER_DS,0,USER_DS,0,USER_DS,0, \
      _LDT(0),0, 0, 0xDFFF, _TSS(0) \
     }, \
     { NULL, } \
    }

// the current task
extern struct task_struct *current;
// the tasks table
extern struct task_struct *task[NR_TASKS];

#endif

