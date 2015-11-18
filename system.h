/*
 * system.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#define save_flags(x) \
    __asm__ __volatile__("pushfl ; popl %0":"=g" (x): /* no input */)

#define restore_flags(x) \
    __asm__ __volatile__("pushl %0 ; popfl": :"g" (x):"memory", "cc")

#define sti() __asm__ ("sti"::)
#define cli() __asm__ ("cli"::)

typedef struct desc_struct {
    unsigned long a, b;
} desc_table[256];

extern desc_table idt, gdt;

#define _set_gate(gate_addr,type,dpl,addr) \
__asm__ ("movw %%dx,%%ax\n\t" \
         "movw %0,%%dx\n\t" \
         "movl %%eax,%1\n\t" \
         "movl %%edx,%2" \
         : \
         : "i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
         "o" (*((char *) (gate_addr))), \
         "o" (*(4+(char *) (gate_addr))), \
         "d" ((char *) (addr)),"a" (0x00080000))

#define set_intr_gate(n,addr) \
    _set_gate(&idt[n], 14, 0, addr)

#define set_system_gate(n,addr) \
    _set_gate(&idt[n],14,3,addr)    // 14 = interrupt disabilitati

#endif
