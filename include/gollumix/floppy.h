#ifndef FLOPPY_H
#define FLOPPY_H

// Device Name
#define FLOPPY_NAME_0 "floppy0"

// Floppy Sector Size
#define FLOPPY_SECTOR_SIZE 512
#define FLOPPY_SECTOR_BITS   9

// Major/Minor number of the device
#define FLOPPY_MAJOR 2
#define FLOPPY_MINOR 0

// IRQ of the device
#define FLOPPY_IRQ   6

// Digital Output Register (DOR) bits (write-only)
// bits 1 and 2 are disc select
#define DOR_REST    0x04
#define DOR_DMA     0x08
#define DOR_MOTA    0x10
#define DOR_MOTB    0x20
#define DOR_MOTC    0x40
#define DOR_MOTD    0x80

// Main Status Register bits (read-only)
#define MSR_ACTA    0x01
#define MSR_ACTB    0x02
#define MSR_ACTC    0x04
#define MSR_ACTD    0x08
#define MSR_BUSY    0x10
#define MSR_NDMA    0x20 
#define MSR_DIO     0x40 
#define MSR_MRQ     0x80 

// Control Configuration Register (write-only)
// bit 2-7 are reserved
#define CCR_500KBS  0
#define CCR_300KBS  1
#define CCR_250KBS  2
#define CCR_1000KBS 3

// Floppy data input register (disk change)
#define DIR_CHAN    (1<<7)

// Floppy status register 0
#define ST0_SE      0x20

// Floppy data commands
#define CMD_WRITE   0xC5
#define CMD_READ    0xE6
#define CMD_FORMAT  0x4D

// Floppy control commands
#define CMD_SPECIFY 0x03
#define CMD_RECAL   0x07
#define CMD_SENSEI  0x08
#define CMD_SEEK    0x0F
#define CMD_VERSION 0x10

// Controller version
#define CTL_VERSION 0x90

struct floppy_address {
    unsigned char head;
    unsigned char track;
    unsigned char sector;
};

// I/O ports where the registers are, for each controller
struct floppy_ports {
    // Common registers
    unsigned long dor,msr,data;
    // AT registers
    unsigned long dir,ccr;
    // PS/2 registers
    unsigned long dsra,dsrb,drs;
};

struct floppy_media {
    unsigned char heads;      // heads
    unsigned char tracks;     // track
    unsigned char spt;        // sectors per track
    unsigned char g3_fmt;     // GAP3 during formatting
    unsigned char g3_rw;      // GAP3 during rw
};

// floppy routines
extern void floppy_init(void);

#endif
