#include "kernel.h"
#include "sched.h"
#include "mm.h"
#include "stddef.h"
#include "errno.h"
#include "segment.h"

int find_empty_process(void) {
    int nr;

    for (nr=1; nr<NR_TASKS; nr++)
        if (task[nr] == NULL)
            return nr;
    return -EAGAIN;
}

asmlinkage int sys_fork(long ebx,   long ecx,   long edx,    long ret,
                        long gs,    long fs,    long es,     long ds,
                        long edi,   long esi,   long ebp,    long __esp, 
                        long __ebx, long __edx, long __ecx,  long eax,
                        long eip,   long cs,    long eflags, long esp,
                        long ss) {
    struct task_struct *tsk;
    int nr;
    long flags;

    save_flags(flags); cli();

    // find a free entry in the process table
    nr = find_empty_process();

    if (nr < 0) {
        printk("fork: pids not available at the moment!\n");
        goto fork_no_entry;
    }

    // allocate a page for the process descriptor
    tsk = (struct task_struct *) get_free_page();

    if (!tsk) {
        goto fork_no_mem;
    }

    // copy descriptor: pid and counter will contain different values for
    // father and child
    *tsk = *current;
    tsk->pid = nr;
    tsk->counter = tsk->priority;

    // setup TSS
    tsk->tss.back_link = 0;
    tsk->tss.esp0 = PAGE_SIZE + (unsigned long) tsk;
    tsk->tss.ss0 = KERNEL_DS;

    tsk->tss.esp1 = 0;
    tsk->tss.ss1 = 0;
    tsk->tss.esp2 = 0;
    tsk->tss.ss2 = 0;
    tsk->tss.cr3 = 0;

    tsk->tss.eip = eip;
    tsk->tss.eflags = eflags;

    tsk->tss.eax = 0;
    tsk->tss.ecx = ecx;
    tsk->tss.edx = edx;
    tsk->tss.ebx = ebx;

    tsk->tss.esp = esp;
    tsk->tss.ebp = ebp;
    tsk->tss.esi = esi;
    tsk->tss.edi = edi;

    tsk->tss.es = es & 0xffff;
    tsk->tss.cs = cs & 0xffff;
    tsk->tss.ss = ss & 0xffff;
    tsk->tss.ds = ds & 0xffff;

    // it is not necessary set FS and GS

    tsk->tss.ldt = _LDT(nr);
    tsk->tss.trace = 0;
    tsk->tss.bitmap = 0xDFFF;
    tsk->tss.tr = _TSS(nr);

    set_tss_desc(gdt+(nr<<1)+FIRST_TSS_ENTRY, &(tsk->tss));
    set_ldt_desc(gdt+(nr<<1)+FIRST_LDT_ENTRY, &(tsk->ldt), 3);
    task[nr] = tsk;

    restore_flags(flags);
    return nr;

fork_no_mem:
        restore_flags(flags);
	    return -ENOMEM;
fork_no_entry:
	    restore_flags(flags);
	    return -EAGAIN;
}

