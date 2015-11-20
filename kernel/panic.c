/*
 * panic.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "kernel.h"

/*
 * This function is used through-out the kernel (includeinh mm and fs)
 * to indicate a major problem.
 */
asmlinkage void panic(char *msgerr) {
    printk("Kernel panic: %s\n",msgerr);
    for(;;);
}
