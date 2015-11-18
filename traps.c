/*
 * traps.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "kernel.h"
#include "system.h"

#define IDT_ELEMENTS (sizeof(idt)/sizeof(struct desc_struct))

extern void _unhand_int(void);

extern void _hwint1(void);

extern void _hwint0(void);  // IRQS
extern void _hwint1(void);
extern void _hwint2(void);
extern void _hwint3(void);
extern void _hwint4(void);
extern void _hwint5(void);
extern void _hwint6(void);
extern void _hwint7(void);
extern void _hwint8(void);
extern void _hwint9(void);
extern void _hwint10(void);
extern void _hwint11(void);
extern void _hwint12(void);
extern void _hwint13(void);
extern void _hwint14(void);
extern void _hwint15(void);

extern void system_call(void);

void unhand_int(void) {
    printk("\nInterrupt with no handler!\n");
    while(1);
}

void init_traps(void) {
    int i;

	// exception: 0x0 - 0x10. Not implemented yet
    for (i=0x0; i<0x20; i++)
        set_intr_gate(i, &_unhand_int);

    set_intr_gate(0x20, &_hwint0);
    set_intr_gate(0x21, &_hwint1);
    set_intr_gate(0x22, &_hwint2);
    set_intr_gate(0x23, &_hwint3);
    set_intr_gate(0x24, &_hwint4);
    set_intr_gate(0x25, &_hwint5);
    set_intr_gate(0x26, &_hwint6);
    set_intr_gate(0x27, &_hwint7);
    set_intr_gate(0x28, &_hwint8);
    set_intr_gate(0x29, &_hwint9);
    set_intr_gate(0x2A, &_hwint10);
    set_intr_gate(0x2B, &_hwint11);
    set_intr_gate(0x2C, &_hwint12);
    set_intr_gate(0x2D, &_hwint13);
    set_intr_gate(0x2E, &_hwint14);
    set_intr_gate(0x2F, &_hwint15);

    for (i=0x30; i<0x7F; i++)
        set_intr_gate(i, &_unhand_int);

    // handle system call using interrupt 0x80
    set_system_gate(0x80, &system_call);

    for (i=0x81; i<IDT_ELEMENTS; i++)
        set_intr_gate(i, &_unhand_int);
}
