#ifndef SCHED_H
#define SCHED_H

#include <gollumix/task.h>
#include <asm/system.h>

#define HZ 100

extern unsigned long volatile jiffies;
extern volatile char need_schedule;

// Entry into gdt where to find first TSS. 0-nul, 1-cs, 2-ds
// 3-TSS0, 4-LDT0, 5-TSS1 etc ...
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

extern void schedule(void);

#endif
