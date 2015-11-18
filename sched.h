#ifndef SCHED_H
#define SCHED_H

#include "tasks.h"
#include "system.h"

#define HZ 100

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]

#define TASK_RUNNING    0

extern unsigned long volatile jiffies;
extern volatile char need_schedule;
extern struct task_struct *current;
extern struct task_struct *task[NR_TASKS];

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

/*
 * This structure represents a process table entry.
 */
struct task_struct {
    /*
     * Do not change these three values.
     */
    long state;      // This is the process state. These are the following
                     // values:
                     //
                     // TASK_RUNNING

    long counter;    // dynamic priority
    long priority;   // static priority

    int pid;         // the process id
    char *mem;       // each process has only one page for code/data.
    struct desc_struct ldt[3];  // local descriptor table of the process
                                // 0:null
                                // 1:CS
                                // 2:DS,SS
    struct tss_struct tss;      // the task state segment of the process
};

/*
 *  INIT_TASK is used to set up the first task table, touch at
 * your own risk!. Base=0, limit=0x9ffff (=640kB)
 */
#define INIT_TASK \
    {TASK_RUNNING, \
     15, \
     15, \
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
     } \
    }

/*
 * Entry into gdt where to find first TSS. 0-nul, 1-cs, 2-ds
 * 3-TSS0, 4-LDT0, 5-TSS1 etc ...
 */
#define FIRST_TSS_ENTRY 3
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)
#define _TSS(n) ((((unsigned long) n)<<4)+(FIRST_TSS_ENTRY<<3))
#define _LDT(n) ((((unsigned long) n)<<4)+(FIRST_LDT_ENTRY<<3))
#define ltr(n) __asm__("ltr %%ax"::"a" (_TSS(n)))
#define lldt(n) __asm__("lldt %%ax"::"a" (_LDT(n)))

/*
 *  switch_to(tsk) should switch tasks to task nr n
 */
#define switch_to(tsk) \
__asm__ __volatile__("cmpl %%ecx,current\n\t" \
    "je 1f\n\t" \
    "movl %%ecx,current\n\t" \
    "ljmp *%0\n\t" \
    "1:" \
    : /* no output */ \
    :"m" (*(((char *)&tsk->tss.tr)-4)), \
    "c" (tsk) \
    :"memory")

#define _set_base(addr,base) \
__asm__("movw %%dx,%0\n\t" \
        "rorl $16,%%edx\n\t" \
        "movb %%dl,%1\n\t" \
        "movb %%dh,%2" \
        ::"m" (*((addr)+2)), \
        "m" (*((addr)+4)), \
        "m" (*((addr)+7)), \
        "d" (base) \
        :"dx")

#define set_base(ldt,base) _set_base( ((char *)&(ldt)) , base )

#define _get_base(addr) ({\
unsigned long __base; \
__asm__("movb %3,%%dh\n\t" \
        "movb %2,%%dl\n\t" \
        "shll $16,%%edx\n\t" \
        "movw %1,%%dx" \
        :"=d" (__base) \
        :"m" (*((addr)+2)), \
        "m" (*((addr)+4)), \
        "m" (*((addr)+7))); \
        __base;})

#define get_base(ldt) _get_base( ((char *)&(ldt)) )

#define get_limit(segment) ({ \
unsigned long __limit; \
__asm__("lsll %1,%0\n\tincl %0":"=r" (__limit):"r" (segment)); \
__limit;})

#endif
