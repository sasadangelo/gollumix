/*
 * Copyright (C) Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 *
 * This code simulates an exec system call. It should be remove when this
 * kernel will support real filesystems.
 */
#include <gollumix/kernel.h>
#include <gollumix/sched.h>
#include <gollumix/mm.h>
#include <gollumix/stddef.h>
#include <gollumix/errno.h>
#include <gollumix/segment.h>
#include <gollumix/ptrace.h>
#include <gollumix/types.h>
#include <asm/string.h>

int find_empty_process(void) {
    int nr;

    for (nr=1; nr<NR_TASKS; nr++)
        if (task[nr] == NULL)
            return nr;
    return -EAGAIN;
}

asmlinkage int sys_fork(struct pt_regs regs) {
    struct task_struct *tsk;
    int nr;
    long flags;
    unsigned long used_memory = 0;

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

    // if we are forking the idle process, we assume its child will call an
    // exec just after the fork. In this case we do not duplicate code/data.
    // If we are forking whatever process, so it is necessary allocate a
    // page for it (containing code and data) e setup its LDT to the new
    // address space
    if (current->pid != 0) {
        // allocate memory for code/data
        tsk->mem = (char *) get_free_pages(current->used_pages);

        if (!tsk->mem) {
            goto fork_data_no_mem;
        }

        // total memory used by current process
        used_memory = current->used_pages * PAGE_SIZE;

        // copy process data
        memcpy(tsk->mem, current->mem, used_memory);

        // set up LDT
        set_code_desc(&(tsk->ldt[1]), (u_long) tsk->mem, used_memory);
        set_data_desc(&(tsk->ldt[2]), (u_long) tsk->mem, used_memory);
    }

    // setup TSS
    tsk->tss.back_link = 0;
    tsk->tss.esp0 = PAGE_SIZE + (unsigned long) tsk;
    tsk->tss.ss0 = KERNEL_DS;

    tsk->tss.esp1 = 0;
    tsk->tss.ss1 = 0;
    tsk->tss.esp2 = 0;
    tsk->tss.ss2 = 0;
    tsk->tss.cr3 = 0;

    tsk->tss.eip = regs.eip;
    tsk->tss.eflags = regs.eflags;

    tsk->tss.eax = 0;
    tsk->tss.ecx = regs.ecx;
    tsk->tss.edx = regs.edx;
    tsk->tss.ebx = regs.ebx;

    tsk->tss.esp = regs.esp;
    tsk->tss.ebp = regs.ebp;
    tsk->tss.esi = regs.esi;
    tsk->tss.edi = regs.edi;

    tsk->tss.es = regs.xes & 0xffff;
    tsk->tss.cs = regs.xcs & 0xffff;
    tsk->tss.ss = regs.xss & 0xffff;
    tsk->tss.ds = regs.xds & 0xffff;

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

fork_data_no_mem:
    free_page(tsk);
fork_no_mem:
    restore_flags(flags);
    return -ENOMEM;
fork_no_entry:
    restore_flags(flags);
    return -EAGAIN;
}

