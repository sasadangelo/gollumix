/*
 * Copyright (C) Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 *
 * This code simulates an exec system call. It should be remove when this
 * kernel will support real filesystems.
 */
#include <gollumix/kernel_map.h>
#include <gollumix/vfs.h>
#include <gollumix/sched.h>
#include <gollumix/mm.h>
#include <gollumix/errno.h>
#include <gollumix/kernel.h>
#include <asm/string.h>
#include <gollumix/types.h>
#include <asm/uaccess.h>
#include <gollumix/ptrace.h>

extern char __KERNEL_END__;

struct file_info {
    char *ptr;          // memory location
    unsigned int size;  // size of the file
}; 

/*
 * This routine find the process on VFS. It returns 1 on successful and 0
 * if it fails.
 */
static int vfs_find_file(char *name, struct file_info *file) {
    struct vfs_header *vfs = (struct vfs_header*) &__KERNEL_END__;
    char *ptr = (char*) &vfs->node[vfs->n_files];
    int i;

    for(i=0; i<vfs->n_files; i++) {
        if (!strcmp(vfs->node[i].name, name)) {
            file->ptr = ptr;
            file->size = vfs->node[i].size;
            return 1;
        }
        ptr += vfs->node[i].size;
    }
    return 0;
}

static int do_exec(char *name, struct pt_regs *regs) {
    unsigned long required_pages, used_memory;
    struct file_info file;
    char *prg_mem;
    long flags;

    save_flags(flags); cli();

    if(!vfs_find_file(name, &file)) {
        goto exec_error_noent;
    }

    required_pages = ((file.size + USER_STACK_SIZE) + PAGE_SIZE -1)/PAGE_SIZE;
    used_memory = required_pages * PAGE_SIZE;

    // allocate a page for code/data/stack
    prg_mem = (char *) get_free_pages(required_pages);

    // if there is no memory available exit immediately
    if (!prg_mem) {
        goto exec_error_nomem;
    }

    // free the process page if possible.
    if (current->mem) {
        free_pages(current->mem, required_pages);
    }

    // clean memory and copy process code/data
    memset(prg_mem, 0, used_memory);
    memcpy(prg_mem, file.ptr, file.size);

	current->mem = prg_mem;
	current->used_pages = required_pages;

    // no tty associated
    current->tty = -1;

    // set up LDT
    set_code_desc(&(current->ldt[1]), (u_long) current->mem, used_memory-1);
    set_data_desc(&(current->ldt[2]), (u_long) current->mem, used_memory-1);

    // set up part of TSS
    regs->eip  = 0;
    regs->eflags = 0x202;  // IF == 1

    regs->eax = 0;         // setup the general purpose registers
    regs->ecx = 0;
    regs->edx = 0;
    regs->ebx = 0;

    regs->esp = used_memory & 0xFFFFFF00;
    regs->ebp = 0;
    regs->esi = 0;
    regs->edi = 0;

    restore_flags(flags);
    return 0;

exec_error_noent:
    restore_flags(flags);
    return -ENOENT;
exec_error_nomem:
    restore_flags(flags);
    return -ENOMEM;
}

/*
 * This routine simulates the exec system call on the kernel side.
 */
asmlinkage int sys_exec(struct pt_regs regs) {
    static char name[VFS_MAX_FILE_NAME+1];
    char *uname = (char *) regs.ebx;

    // copy the program name in user space
    strncpy_from_user(name, uname, sizeof(name));

	return do_exec(name, &regs);
}

