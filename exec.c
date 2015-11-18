/*
 * Copyright (C) Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 *
 * This code simulates an exec system call. It should be remove when this
 * kernel will support real filesystems.
 */
#include "kernel_map.h"
#include "vfs.h"
#include "sched.h"
#include "mm.h"
#include "errno.h"
#include "kernel.h"
#include "string.h"
#include "types.h"
#include "uaccess.h"
#include "ptrace.h"

struct file_info {
    char *ptr;          // memory location
    unsigned int size;  // size of the file
};

static int exec_proc(struct file_info *file, struct pt_regs *regs) {
    char *prg_mem;

    // 1/3 is for stack!
    if (file->size > PAGE_SIZE*2/3) {
        printk ("process too big!");
        return -ENOMEM;
    }

    // allocate a page for code/data/stack
    prg_mem = (char *) get_free_page();

    // if there is no memory available exit immediately
    if (!prg_mem) {
        return -ENOMEM;
    }

    // free the process page if possible.
    if (current->mem) {
        free_page(current->mem);
    }

    // clean memory and copy process code/data
    memset(prg_mem, 0, PAGE_SIZE-1);
    memcpy(prg_mem, file->ptr, PAGE_SIZE-1);

	current->mem = prg_mem;

    // set up LDT
    set_code_desc(&(current->ldt[1]), (u_long) current->mem, PAGE_SIZE-1);
    set_data_desc(&(current->ldt[2]), (u_long) current->mem, PAGE_SIZE-1);

    // set up part of TSS
    regs->eip  = 0;
    regs->eflags = 0x202;  // IF == 1

    regs->eax = 0;         // setup the general purpose registers
    regs->ecx = 0;
    regs->edx = 0;
    regs->ebx = 0;

    regs->esp = PAGE_SIZE & 0xFFFFFF00;
    regs->ebp = 0;
    regs->esi = 0;
    regs->edi = 0;

    return 0;
}

/*
 * This routine find the process on VFS. It returns 1 on successful and 0
 * if it fails.
 */
static int vfs_find_file(char *name, struct file_info *file) {
    struct vfs_header *vfs = (struct vfs_header*) K_REAL_END;
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

static char tmp_name[VFS_MAX_FILE_NAME+1];

/*
 * This routine simulates the exec system call on the kernel side.
 */
asmlinkage int sys_exec(struct pt_regs regs) {
    char *name = (char *) regs.ebx;
    struct file_info file;
    long flags;
    int i, ret;

    save_flags(flags); cli();

    // copy filename from user space
    // TODO: replace this code with the copy from user
    for(i=0; i<=VFS_MAX_FILE_NAME; i++) {
        tmp_name[i] = (char) get_fs_byte(&name[i]);
        if (tmp_name[i] == '\0') break;
    }

    if(!vfs_find_file(tmp_name, &file))
        goto exec_error_exit;

    ret = exec_proc(&file, &regs);

    restore_flags(flags);
    return ret;

exec_error_exit:
    restore_flags(flags);
    return -ENOENT;
}

