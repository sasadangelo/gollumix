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

#define _set_tssldt_desc(n,addr,limit,type) \
    __asm__ __volatile__ ("movw $" #limit ",%1\n\t" \
    "movw %%ax,%2\n\t" \
    "rorl $16,%%eax\n\t" \
    "movb %%al,%3\n\t" \
    "movb $" type ",%4\n\t" \
    "movb $0x00,%5\n\t" \
    "movb %%ah,%6\n\t" \
    "rorl $16,%%eax" \
    : /* no output */ \
    :"a" (addr), "m" (*(n)), "m" (*(n+2)), "m" (*(n+4)), \
    "m" (*(n+5)), "m" (*(n+6)), "m" (*(n+7)) \
    )

#define set_tss_desc(n,addr) \
    _set_tssldt_desc(((char *) (n)),((int)(addr)),103,"0x89")
#define set_ldt_desc(n,addr,size) \
    _set_tssldt_desc(((char *) (n)),((int)(addr)),((size << 3) - 1),"0x82")

#endif
