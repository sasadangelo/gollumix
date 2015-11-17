#ifndef SYSTEM_H
#define SYSTEM_H

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

#endif
