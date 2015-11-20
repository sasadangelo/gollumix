#include "io.h"
#include "kernel.h"
#include "serial.h"
#include "irq.h"

// COMx base address
#define COM_BASE_ADDR 0x400

#define DLAB_ON       0x80

// UART Registers
#define UART_IER_REG  1
#define UART_IIR_REG  2
#define UART_FCR_REG  2
#define UART_LCR_REG  3
#define UART_MCR_REG  4
#define UART_LSR_REG  5
#define UART_SCR      7

#define UART_LSR_DHRE 0x40    // Empty data holding register
#define UART_LSR_THRE 0x20    // Empty transmitter holding register
#define EMPTY_HOLDING_REGS (UART_LSR_DHRE | UART_LSR_THRE)

#define DIVISOR_LATCH_HIGH_SHIFT   8
#define DIVISOR_LATCH_LOW_MASK     0xff

#define PARITY_SHIFT   3
#define STOP_SHIFT     2

#define ODD_PARITY     1 << PARITY_SHIFT
#define EVEN_PARITY    3 << PARITY_SHIFT
#define HIGH_PARITY    5 << PARITY_SHIFT
#define LOW_PARITY     7 << PARITY_SHIFT

#define TWO_STOP_BITS  1 << STOP_SHIFT

// UART PORT TYPE
#define PORT_UNKNOWN   0
#define PORT_8250      1
#define PORT_16450     2

static void rs_interrupt(void);

// RS table - Type is defaults to PORT_UNKNOWN
struct serial_struct rs_table[] = {
    // type          minor   base IRQ flags  speed          ISR
    { PORT_UNKNOWN,     0, 0x3f8,  4,    0,     0, rs_interrupt }
};

// get a character from the serial line
static unsigned int serial_in(struct serial_struct *port, int offset) {
    return inb(port->base + offset);
}

static unsigned int serial_getchar(struct serial_struct *port) {
    return serial_in(port, 0); 
}               

static void wait_for_xmitr(struct serial_struct *port)
{
    unsigned int status, tmout = 1000000;
    do {
        status = serial_in(port, UART_LSR_REG);

        if (--tmout == 0) {
            break;
        }
    } while((status & EMPTY_HOLDING_REGS) != EMPTY_HOLDING_REGS);
}

// push a byte on the serial line
static void serial_out(unsigned int value, struct serial_struct *port,
                       int offset) {
    wait_for_xmitr(port);
    outb(value, port->base + offset);
}

// push a byte on the serial line
static void serial_outp(unsigned int value, struct serial_struct *port,
                        int offset) {
    wait_for_xmitr(port);
    outb_p(value, port->base + offset);
}

// push a character on the serial line
static void serial_putchar(char c, struct serial_struct *port) {
    serial_out(c, port, 0);
}

// write the buffer on the serial line
void rs_write(struct tty_struct *tty) {
    char c;

    while((c = getch(&tty->write_q)) >= 0) {
        serial_putchar(c, &rs_table[0]);
    }
}

// Interrupt Service Routine
static void rs_interrupt(void) {
    unsigned int c, ch;
    
    do {
        c = serial_in(&rs_table[0], UART_LSR_REG);
        if (c & 1) {
            ch = serial_getchar(&rs_table[0]);
            put_queue(&tty_table[6].read_q, ch);
        }
    } while(c & 1);
}

// port probing
static int rs_setup(struct serial_struct *port) {
    int divisor, c, scratch, status1, status2, retval;

    divisor = 115200 / port->speed;
    // turn off interrupt
    serial_out(0x0, port, UART_IER_REG);
    // set DLAB ON
    serial_out(DLAB_ON, port, UART_LCR_REG);
    // set the BAUD RATE
    serial_out(divisor & DIVISOR_LATCH_LOW_MASK, port, 0);
    serial_out(divisor >> DIVISOR_LATCH_HIGH_SHIFT, port, UART_IER_REG);
    // set Bits, Parity and number of Stop bits
    serial_out(port->flags, port, UART_LCR_REG);
    // set FIFO size
    serial_out(0xc7, port, UART_FCR_REG);
    // turn on DTR, RTS and OUT2
    serial_out(0x0b, port, UART_MCR_REG);

    // Probing UART model
    if (serial_in(port, UART_LSR_REG) == 0xff) {
        return -1;
    }

    retval = 0;

    c = serial_in(port, UART_IIR_REG);
    if (c & 0xc0) {
        scratch = serial_in(port, UART_SCR);
        serial_outp(0xa5, port, UART_SCR);
        status1 = serial_in(port, UART_SCR);
        serial_outp(0x5a, port, UART_SCR);
        status2 = serial_in(port, UART_SCR);
        serial_outp(scratch, port, UART_SCR);

        if ((status1 != 0xa5) || (status2 != 0x5a))
            port->type = PORT_8250;
        else
            port->type = PORT_16450;
    } else {
        port->type = PORT_UNKNOWN;
        //retval = -1;
    }

    // set the interrupt handler for the current serial line
    add_irq_handler(port->irq, port->isr);

    // turn on interrupt for the current serial line
    serial_out(0x01, port, UART_IER_REG);

    //return retval;
    return 0;
}

// COMx is available, register its parameter
static int rs_register(struct serial_struct *port, int baud, int bits,
                       int parity, int stop_bits) {
    int flags, retval;

    flags = bits - 5;
    switch (parity) {
    case 'n':
        break;
    case 'o':
        flags |= ODD_PARITY;
        break;
    case 'e':
        flags |= EVEN_PARITY;
        break;
    case 'h':
        flags |= HIGH_PARITY;
        break;
    case 'l':
        flags |= LOW_PARITY;
        break;
    default:
        // default is 'n' so exit from switch
        break;
    }
						
    if (stop_bits == 2) {
        flags |= TWO_STOP_BITS;
    }
									
    port->flags = flags;
    port->speed = baud;
											
    retval =  rs_setup(port);
												
    if (retval) {
        printk("serial_port_setup exit with return code: %d.\n", retval);
    } else {
        printk("Registered ttyS%d (%d, %d%c%d) Base: 0x%x IRQ: %d ",
           port->minor, port->speed, bits, parity, stop_bits,
           port->base, port->irq);

        switch(port->type) {
        case PORT_8250:
            printk("[UART 8250].\n");
            break;
        case PORT_16450:
            printk("[UART 16450].\n");
            break;
        default:
            printk("[UART UNKNOWN].\n");
        }
    }

    return retval;
}

// This routine initialize the two serial port COM1 and COM2 (if available)
void rs_init(void) {
    short *ptraddr;
    int i;

    int baud = 38400;
    int bits   = 8;
    int parity = 'n';
    int stop_bits = 1;

    ptraddr = (unsigned short *) COM_BASE_ADDR;

    // COM1 base address = 0x400
    // COM2 base address = 0x402
    // COM3 base address = 0x404
    // COM4 base address = 0x406
    //
    // If COMx base address is 0 then the port is not present.
    for (i = 0; i < N_SERIALS; i++ , *ptraddr++) {
        if (*ptraddr) {
            // register serial port
            rs_register(&rs_table[i], baud, bits, parity, stop_bits);
        } else {
            // port not available
            rs_table[i].base = 0;
            rs_table[i].irq  = 0;
        }
    }
}
