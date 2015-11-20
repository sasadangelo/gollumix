#include <gollumix/dma.h>
#include <gollumix/floppy.h>
#include <gollumix/fs.h>
#include <gollumix/blkdev.h>
#include <gollumix/stddef.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <gollumix/task.h>
#include <gollumix/errno.h>
#include <gollumix/kernel.h>
#include <gollumix/irq.h>
#include <gollumix/delay.h>

// number of try for rw/seek/IO operations
#define MAX_RW_TRIES   3
#define MAX_SEEK_TRIES 3
#define MAX_FDC_TRIES  10000

// max number of result returned by a fdc command
#define MAX_PARAMETERS 8

volatile bool interrupt_pending = true;
static unsigned char curr_track = 0;
static unsigned char curr_DOR   = DOR_DMA | DOR_REST;
static bool motor_on            = false;

static int  fdc_open (struct file * filp);
static void fdc_close(struct file * filp);
static int  fdc_read (struct file * filp, char * buf, int count);
static int  fdc_write(struct file * filp, char * buf, int count);

// floppy queue. It is necessary since when a process send a command to the
// floppy controller, it should wait for response.
static struct wait_queue fdc_queue;

// the status register
static unsigned char status[MAX_PARAMETERS];

// floppy geometries
struct floppy_media geom[]={ {2, 80, 18, 0x54, 0x1b} }; // 1.44M floppy

// I/O ports where the registers are, for each controller
struct floppy_ports dprts[] = {
    // PC primary controller
    {0x3f2, 0x3f4, 0x3f5, 0x3f7, 0x3f7, 0x3f0, 0x3f1, 0x3f4},
    // PC secondary controller
    {0x372, 0x374, 0x375, 0x377, 0x377, 0x370, 0x371, 0x374},
};

static struct file_operations floppy_fops = {
    fdc_open,
    fdc_read,
    fdc_write,
    fdc_close
};

struct blkdev blk_floppy = {
    .next        = NULL,
    .name        = FLOPPY_NAME_0,
    .sectsize    = FLOPPY_SECTOR_SIZE,
    .sectbits    = FLOPPY_SECTOR_BITS,
    .major       = FLOPPY_MAJOR,
    .minor       = FLOPPY_MINOR,
    .blkdev_fops = &floppy_fops
};

// Convert a block number to a floppy address (head, track, sector).
static void floppy_block(unsigned long block, struct floppy_address *addr) {
    addr->head = (block % (geom[0].spt * geom[0].heads)) / (geom[0].spt);
    addr->track = block / (geom[0].spt * geom[0].heads);
    addr->sector = block % geom[0].spt + 1;
}   

// Routine used to send a byte to the Floppy Controller through the Data
// Register (DR)
static bool fdc_send(struct floppy_ports *p, unsigned char b) {
    unsigned char msr;
    int retries;

    // Try the operation for a finite number of time.

    // To send a byte to the floppy controller it is necessary check the
    // Main Status Register (MSR). Basically if the bit 7 (MRQ) is 1 and bit 6
    // (DIO) is 0 (CPU->Controller), then the controller is ready to receive
    // data.
    for(retries=0; retries<MAX_FDC_TRIES; ++retries) {
        msr = inb(p->msr) & (MSR_MRQ | MSR_DIO);
        // check if controller is ready to receive data
        if (msr == MSR_MRQ) {
            // ok, controller is ready. Send the byte through the data register.
            outb(b, p->data);
            return true;
        }
    }

    // controller is not listening. A reset is needed
    printk("Floppy 0: unable to send %x to FDC.\n", b);
    return false;
}

// Floppy ISR
static void floppy_isr(void) {
    interrupt_pending=false;
    // if there are processes waiting, wake up them
    wake_up(&fdc_queue);
}

static int fdc_open(struct file * filp) {
    return 0;
}

static void fdc_close(struct file * filp) {
}

static int fdc_write(struct file * filp, char *buf, int count) {
    printk("Floppy 0: read-only device\n");
    return -ENOTSUPP;
}

static void wait_interrupt(void) {
    while(interrupt_pending);
    interrupt_pending = true;
}

static void fdc_start_motor(void) {
    unsigned long flags;

    // if motor is not active, please turn it on
    if (!motor_on) {
        save_flags(flags); cli();
        curr_DOR |= DOR_MOTA;
        outb(curr_DOR, dprts->dor);
        motor_on = true;
        restore_flags(flags);

        // the time necessary to spin the motor on is about 500 ms
        mdelay(500);
    }
}

static void fdc_stop_motor(void) {
    // if motor is active, please turn it off
    if (motor_on) {
        curr_DOR &= ~DOR_MOTA;
        outb(curr_DOR, dprts->dor);
        motor_on = false;
    }
}

// this routine reset the floppy disk controller
static void fdc_reset(struct floppy_ports *p) {
    unsigned long flags;

    save_flags(flags); cli();

    // reset the floppy
    outb(curr_DOR & ~0x04, p->dor);

    // a small delay
    mdelay(18);

    // enable DMA and IRQ
    outb(curr_DOR, p->dor);

    // set floppy data transfer to 500 Kbs
    outb(CCR_500KBS, p->ccr);

    restore_flags(flags);
    wait_interrupt();
}

// this routine extract from the controller the result of a command
static bool fdc_result(void) {
    unsigned char params = 0, msr;
    int retries;

    for(retries=0; retries<MAX_FDC_TRIES; ++retries) {
        msr = inb(dprts->msr) & (MSR_MRQ | MSR_DIO | MSR_BUSY);

        if (msr == MSR_MRQ) {
            // ok all data have been read
            return true;
        }

        if (msr == (MSR_MRQ | MSR_DIO | MSR_BUSY)) {
            if (params >= MAX_PARAMETERS) {
                printk("Floppy 0: too many parameters returned.\n");
                break;
            }

            status[params++] = inb(dprts->data);
        }
    }

    printk("Floppy 0: timeout.\n");
    return false;
}

// Check interrupt status.
static bool fdc_sensei(void) {
    unsigned long flags;

    save_flags(flags); cli();

    // issue a SENSE Interrupt command
    if (!fdc_send(dprts, CMD_SENSEI)) {
        restore_flags(flags);
        return false;
    }

    if (!fdc_result()) {
        restore_flags(flags);
        return false;
    }

    restore_flags(flags);
    return true;
}

static bool fdc_seek(struct floppy_ports *p,
                     unsigned char head,
                     unsigned char trk) {
    unsigned long flags;

    // if the head is already positioned on the input track no actions are
    // performed
    if ( trk==curr_track) {
        return true;
    }

    fdc_start_motor();

    save_flags(flags); cli();

    // Send the seek command
    if (!fdc_send(p, CMD_SEEK)  || 
        !fdc_send(p, head << 2) || 
        !fdc_send(p, trk)) {
        restore_flags(flags);
        return false;
    }

    restore_flags(flags);

    // wait for interrupt
    sleep(&fdc_queue);

    // issue a SENSE Interrupt command
    if (!fdc_sensei()) {
        return false;
    }

    curr_track = status[1];

    // check if seek has been completed correctly
    if (!(status[0] & ST0_SE)) {
        printk("Floppy 0: seek failed\n");
        return false;
    }

    if (curr_track != trk) {
        printk("Floppy 0: seek to %u failed (%u)\n", trk, curr_track);
        return false;
    }

    return true;
}

static bool fdc_recalibrate(struct floppy_ports *p) {
    unsigned long flags;

    save_flags(flags); cli();

    // if motor A is not active, please turn it on
    //fdc_start_motor();

    // send the recalibrate command to the controller
    if (!fdc_send(p, CMD_RECAL) || !fdc_send(p, 0)) {
        restore_flags(flags);
        return false;
    }

    restore_flags(flags);

    // wait for interrupt
    sleep(&fdc_queue);

    // issue a SENSE Interrupt command
    if (!fdc_sensei()) {
        return false;
    }

    curr_track = status[1];

    // check if seek has been completed correctly
    if ((status[0] & 0xF8) != ST0_SE || curr_track != 0) { 
        printk("Floppy 0: recalibration failed (%u).\n", curr_track);
        return false;
    }

    return true;
}

static int fdc_read_block(unsigned long block, char *buf, int count) {

    unsigned char rw_tries = 0, seek_tries = 0;
    struct floppy_address address;
    unsigned long flags;

    fdc_start_motor();

    // set floppy data transfer to 500 Kbs
    outb(CCR_500KBS, dprts->ccr);

    // convert the block in disk address
    floppy_block(block, &address);

    // The read operation is repeated three times. If three failure occurs
    // a recalibration is performed and the read operation is repeated again
    // for max three times. This process is repeated MAX_SEEK_TRIES times.
    do {
        if (!fdc_seek(dprts, address.head, address.track)) {
            fdc_stop_motor();
            return -ESPIPE;
        }

        // the read operation is tried max MAX_RW_TRIES times
        do {
            save_flags(flags); cli();

            dma_read(2, buf, FLOPPY_SECTOR_SIZE);

            if (!fdc_send(dprts, CMD_READ)          ||
                !fdc_send(dprts, address.head << 2) ||
                !fdc_send(dprts, address.track)     ||
                !fdc_send(dprts, address.head)      ||
                !fdc_send(dprts, address.sector)    ||
                !fdc_send(dprts, 2)                 ||
                !fdc_send(dprts, geom[0].spt)       ||
                !fdc_send(dprts, geom[0].g3_rw)     ||
                !fdc_send(dprts, 0xff)) {

                restore_flags(flags);
                fdc_stop_motor();
                return -1;
            }


            restore_flags(flags);

            // wait for completion
            sleep(&fdc_queue);

            save_flags(flags); cli();

            if (!fdc_result()) {
                restore_flags(flags);
                fdc_stop_motor();
                return -1;
            }

            restore_flags(flags);

            curr_track = status[3];

            // check for successful of read operation
            if ((status[0] & 0xC0) == 0) {
                fdc_stop_motor();
                return FLOPPY_SECTOR_SIZE;
            }
        } while(rw_tries++ < MAX_RW_TRIES);

        // FROM 82077AA CHMOS SINGLE-CHIP Floppy Disk Controller:
        // a persisting failure could indicate the seek operation did not 
        // achieve proper alignment between the head and the track.
        // The disk head should then be recalibrated and the seek repeated for
        // a max of two more tries.
        if (!fdc_recalibrate(dprts)) {
            fdc_stop_motor();
            return -ESPIPE;
        }

    } while(++seek_tries < MAX_SEEK_TRIES);

    fdc_stop_motor();
    return -1;
}

static int fdc_read(struct file *filp, char *buf, int count) {
    unsigned long block_nr = filp->f_pos >> blk_floppy.sectbits;
    unsigned long start_offset = filp->f_pos % blk_floppy.sectsize;
    char buffer[FLOPPY_SECTOR_SIZE];
    int bytes_read = 0, bytes_useful, result;

    while (count > 0) {
        result = fdc_read_block(block_nr, buffer, FLOPPY_SECTOR_SIZE);

        if (result < 0) return result;

        if (count > FLOPPY_SECTOR_SIZE) {
            bytes_useful = FLOPPY_SECTOR_SIZE;
        } else {
            bytes_useful = count;
        }

        // move data in user space
        copy_to_user(buf + bytes_read, (const char *) buffer + start_offset, 
                     bytes_useful);

        bytes_read += bytes_useful;
        count -= bytes_useful;

        start_offset = 0;
        ++block_nr;
    }

    filp->f_pos += bytes_read;
    return bytes_read;
}

static bool fdc_specify(unsigned char s1, unsigned char s2) {
    unsigned long flags;

    save_flags(flags); cli();

    if (!fdc_send(dprts, CMD_SPECIFY) ||
        !fdc_send(dprts, s1)        ||
        !fdc_send(dprts, s2)) {

        restore_flags(flags);
        return false;
    }
    
    restore_flags(flags);
    return true;
}

// This routine implements the initialization flow chart reported in the Intel 
// 82077AA CHMOS Single-Chip Floppy Disk Controller data sheet. Please check
// the documentation for more details.
static bool fdc_initialize(void) {
    unsigned char i;

    // reset floppy controller
    fdc_reset(dprts);

    // issue a SENSE Interrupt command for each drive
    for (i=0; i<4; ++i) {
        if (!fdc_sensei()) {
            return false;
        }

        // set the track for drive 0
        if (i == 0) curr_track = status[1];
    }

    // set the FDC parameters: step rate, head load time and head unload time
    // Step Rate        = 0x0  => Step Rate        =   3 ms
    // Head Unload Time = 0xF  => Head Unload Time = 240 ms
    // Head Load Time   = 0x01 => Head Load Time   =   2 ms
    //
    // Please see 
    // http://debs.future.easyspace.com/Programming/Hardware/FDC/floppy.html
    if (!fdc_specify(0x0F, 0x02)) {
        return false;
    }

    return true;
}

void floppy_init(void) {
    blkdev_add(&blk_floppy);
    add_irq_handler(FLOPPY_IRQ, &floppy_isr);

    blkdev_fops[0] = &floppy_fops;

    printk("Floppy 0: FDC initialization ... ");

    // initialize the floppy disk controller
    if (!fdc_initialize()) {
        // initialization failed
        printk("failed.\n");
        return;
    }

    printk("ok.\n");
}
