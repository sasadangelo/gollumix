/*
 * kernel.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef KERNEL_H
#define KERNEL_H

#define asmlinkage	__attribute__((regparm(0)))

asmlinkage int printk(const char * fmt, ...);
asmlinkage void panic(char *msgerr);

#endif
