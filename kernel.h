#ifndef KERNEL_H
#define KERNEL_H

#define asmlinkage	__attribute__((regparm(0)))

asmlinkage int printk(const char * fmt, ...);

#endif