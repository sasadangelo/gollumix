/*
 * system.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <gollumix/segment.h>

#define save_flags(x) \
    __asm__ __volatile__("pushfl ; popl %0":"=g" (x): /* no input */)

#define restore_flags(x) \
    __asm__ __volatile__("pushl %0 ; popfl": :"g" (x):"memory", "cc")

#define sti() __asm__ ("sti": : :"memory")
#define cli() __asm__ ("cli": : :"memory")

#define move_to_user_mode() \
__asm__ __volatile__ ("movl %%esp,%%eax\n\t" \
    "pushl %0\n\t" \
    "pushl %%eax\n\t" \
    "pushfl\n\t" \
    "pushl %1\n\t" \
    "pushl $1f\n\t" \
    "iret\n" \
    "1:\tmovl %0,%%eax\n\t" \
    "mov %%ax,%%ds\n\t" \
    "mov %%ax,%%es\n\t" \
    "mov %%ax,%%fs\n\t" \
    "mov %%ax,%%gs" \
    : /* no outputs */ :"i" (USER_DS), "i" (USER_CS):"%eax")

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

extern inline void
set_code_desc(void *_desc, unsigned long base, unsigned long limit) {
    long *desc = (long*)_desc;

    *(desc+1) = (base & 0xff000000) |
                ((base & 0x00ff0000)>>16) |
                (limit & 0x000f0000) |
                (0x0040FA00);

    *(desc) = ((base & 0x0000ffff)<<16) |
                (limit & 0x0000ffff);
}

extern inline void
set_data_desc(void *_desc, unsigned long base, unsigned long limit) {
    long *desc = (long*)_desc;

    *(desc+1) = (base & 0xff000000) |
                ((base & 0x00ff0000)>>16) |
                (limit & 0x000f0000) |
                (0x0040F200);

    *(desc) = ((base & 0x0000ffff)<<16) |
                (limit & 0x0000ffff);
}

extern inline void put_fs_byte(char val,char *addr) {
    __asm__ ("movb %0,%%fs:%1"::"q" (val),"m" (*addr));
}

#endif
