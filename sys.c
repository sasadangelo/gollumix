/*
 * sys.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "kernel.h"
#include "sys.h"
#include "unistd.h"
#include "ptrace.h"

/*
 * For this step only this system call is supported for debug purpose.
 */
extern asmlinkage int sys_print(char *msg);
extern asmlinkage int sys_fork(struct pt_regs regs);
extern asmlinkage int sys_exec(char *filename);

/*
 * The system call table
 */
void *sys_call_table[NR_SYSCALLS] = 
    { sys_print, sys_fork, sys_exec };