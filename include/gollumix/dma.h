#ifndef DMA_H
#define DMA_H

struct dma {
    unsigned char page;
    unsigned char addr;
    unsigned char count;
    unsigned char mask;
    unsigned char mode;
    unsigned char clear;
};

// DMA Interface
extern void dma_read (unsigned char chan, void *addr, unsigned short len);
extern void dma_write(unsigned char chan, void *addr, unsigned short len);

#endif
