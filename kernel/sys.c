/*
 * sys.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <gollumix/kernel.h>
#include <gollumix/sys.h>
#include <gollumix/unistd.h>
#include <gollumix/ptrace.h>

/*
 * For this step only this system call is supported for debug purpose.
 */
extern asmlinkage int sys_print(char *msg);
extern asmlinkage int sys_fork(struct pt_regs regs);
extern asmlinkage int sys_exec(char *filename);
extern asmlinkage int sys_open(const char *pathname);
extern asmlinkage int sys_close(int fd);
extern asmlinkage ssize_t sys_read(int fd, void *buf, size_t count);
extern asmlinkage ssize_t sys_write(int fd, void *buf, size_t count);

/*
 * The system call table
 */
void *sys_call_table[NR_SYSCALLS] = 
    { sys_print, sys_fork, sys_exec, sys_open, sys_close, sys_read, sys_write };
