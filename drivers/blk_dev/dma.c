#include <asm/system.h>
#include <asm/io.h>
#include <gollumix/dma.h>

struct dma dma_chan[] = {
    // 8 bit DMA channels
    {0x87, 0x00, 0x01,  0x0a, 0x0b, 0x0c},
    {0x83, 0x02, 0x03,  0x0a, 0x0b, 0x0c},
    {0x81, 0x04, 0x05,  0x0a, 0x0b, 0x0c},
    {0x82, 0x06, 0x07,  0x0a, 0x0b, 0x0c},

    // 16 bit DMA channels
    {0x8f, 0xc0, 0xc2,  0xd4, 0xd6, 0xd8},
    {0x8b, 0xc6, 0xc6,  0xd4, 0xd6, 0xd8},
    {0x89, 0xc8, 0xca,  0xd4, 0xd6, 0xd8},
    {0x8a, 0xcc, 0xce,  0xd4, 0xd6, 0xd8},
//  {page, addr, count, mask, mode, clear},
};

// DMA Private Routines
static void dma_cmd(unsigned char  chan, 
                    unsigned char  page, 
                    unsigned short ofs, 
                    unsigned short len, 
                    unsigned char  mode) {
    unsigned long flags;

    save_flags(flags); cli();

    // mask that sucker
    outb(0x04|chan, dma_chan[chan].mask);

    // clear any current transfers
    outb(0x00, dma_chan[chan].clear);

    // send the mode to the controller
    outb(mode, dma_chan[chan].mode);

    // send the offset address byte at a time
    outb(ofs & 0x00FF, dma_chan[chan].addr);
    outb((ofs&0xFF00)>>8, dma_chan[chan].addr);

    // send the page
    outb(page, dma_chan[chan].page);

    // send data length
    outb(len & 0x00FF, dma_chan[chan].count);
    outb((len & 0xFF00) >> 8, dma_chan[chan].count);

    // unmask her
    outb(chan, dma_chan[chan].mask);

    restore_flags(flags);
}

void dma_write(unsigned char chan, void *addr, unsigned short len) {
    unsigned long a=(unsigned long)addr;
    dma_cmd(chan, a>>16, a & 0xffff, len-1, 0x48+chan);
}

void dma_read(unsigned char chan, void *addr, unsigned short len) {
    unsigned long a=(unsigned long)addr;
    dma_cmd(chan, a>>16, a & 0xffff, len-1, 0x44+chan);
}

