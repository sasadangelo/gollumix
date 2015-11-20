/*
 * traps.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include "kernel.h"
#include "system.h"
#include "ptrace.h"
#include "task.h"

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

extern void _exception0x00(void);       // Traps
extern void _exception0x01(void);
extern void _exception0x02(void);
extern void _exception0x03(void);
extern void _exception0x04(void);
extern void _exception0x05(void);
extern void _exception0x06(void);
extern void _exception0x07(void);
extern void _exception0x08(void);
extern void _exception0x09(void);
extern void _exception0x0a(void);
extern void _exception0x0b(void);
extern void _exception0x0c(void);
extern void _exception0x0d(void);
extern void _exception0x0e(void);
extern void _exception0x0f(void);
extern void _exception0x10(void);

extern void system_call(void);

void c_exception(int n, struct pt_regs regs) {
    if (n==2) {
        panic ("NMI interrupt!");
    }

    printk("\nException %d\n", n);
    printk("\tCS = 0x%02X  EIP = 0x%08X\n\t", regs.xcs, regs.eip);

    if (regs.xcs == USER_CS) {
        // TODO
        // The exception occured inside a process, so it must be killed.
        // Our kernel does not support signals and exit sys call yes, so
        // we block the whole kernel.
        printk("in the process %d\n\t", current->pid);
        panic("The process should be killed and kernel still running."
              "System HALTED!\n");
    } else if (regs.xcs == KERNEL_CS) {
        panic("in kernel mode, system HALTED!");
    } else {
        panic("something is really wrong, system HALTED!");
    }
}

void unhand_int(void) {
    printk("\nInterrupt with no handler!\n");
    while(1);
}

void init_traps(void) {
    int i;

// cpu exceptions:
    set_intr_gate(0x00, &_exception0x00);
    set_intr_gate(0x01, &_exception0x01);
    set_intr_gate(0x02, &_exception0x02);            // NMI
    set_intr_gate(0x03, &_exception0x03);
    set_intr_gate(0x04, &_exception0x04);
    set_intr_gate(0x05, &_exception0x05);
    set_intr_gate(0x06, &_exception0x06);
    set_intr_gate(0x07, &_exception0x07);
    set_intr_gate(0x08, &_exception0x08);
    set_intr_gate(0x09, &_exception0x09);
    set_intr_gate(0x0A, &_exception0x0a);
    set_intr_gate(0x0B, &_exception0x0b);
    set_intr_gate(0x0C, &_exception0x0c);
    set_intr_gate(0x0D, &_exception0x0d);
    set_intr_gate(0x0E, &_exception0x0e);
    set_intr_gate(0x0F, &_exception0x0f);
    set_intr_gate(0x10, &_exception0x10);

    // interrupts between 0x11 and 0x20 are not used
    for (i=0x11; i<0x20; i++)
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

    // interrupts between 0x30 and 0x7F are not used
    for (i=0x30; i<0x7F; i++)
        set_intr_gate(i, &_unhand_int);

    // handle system call using interrupt 0x80
    set_system_gate(0x80, &system_call);

    // interrupts between 0x81 and 0xFF are not used 
    for (i=0x81; i<IDT_ELEMENTS; i++)
        set_intr_gate(i, &_unhand_int);
}
