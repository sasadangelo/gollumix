#include "kernel.h"

// COMx base address
#define COM_BASE_ADDR 0x400
// RS table size
#define RS_TABLE_LEN  1

// This routine initialize the serial port COM1
void rs_init(void) {
	short *ptraddr;
    int i;

	ptraddr = (unsigned short *) COM_BASE_ADDR;

    // COM1 base address = 0x400
    // COM2 base address = 0x402
    // COM3 base address = 0x404
    // COM4 base address = 0x406
    //
    // If COMx base address is 0 then the port is not present.
    for (i = 0; i < RS_TABLE_LEN; i++ , *ptraddr++) {
		if (*ptraddr) {
            printk("COM%d is available.\n", i+1);
		} else {
            printk("COM%d is not available.\n", i+1);
		}
	}
}

