/*
 * sys.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "kernel.h"
#include "sys.h"
#include "unistd.h"

/*
 * For this step only this system call is supported for debug purpose.
 */
extern asmlinkage int sys_print(char *msg);
extern asmlinkage int sys_fork(void);

/*
 * The system call table
 */
void *sys_call_table[NR_SYSCALLS] = 
    { sys_print, 
      sys_fork };
