/*
 * mm.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#include "kernel.h"
#include "kernel_map.h"
#include "mm.h"
#include "stddef.h"

#define BITS_PER_BYTE 8
#define MEM_INIT	  K_END
#define NR_PAGES	  64
#define MAP_SIZE	  ((NR_PAGES+BITS_PER_BYTE-1)/BITS_PER_BYTE)

/*
 * The memory bitmap. Each bit represents a memory page. If bit i == 1, the
 * page i is allocated, otherise it is available for allocation.
 */
static unsigned char map[MAP_SIZE] = {0, };

/*
 * Set the n bit of the bitmap to 1, in order to indicate that the page
 * has been allocated.
 */
static inline void set_bit(int n) {
    unsigned char *m = map;

    m += n/BITS_PER_BYTE;
    n %= BITS_PER_BYTE;
    *m |= 1<<n;
}

/*
 * Clear the n bit in the bitmap, in order to indicate that the n page has
 * been released.
 */
static inline void clear_bit(int n) {
    unsigned char *m = map;

    m += n/BITS_PER_BYTE;
    n %= BITS_PER_BYTE;
    *m &= ~(1<<n);
}

/*
 * Get the state of the n-th bit in the bitmap.
 */
static inline int get_bit(int n) {
    unsigned char *m = map;

    m += n/BITS_PER_BYTE;
    n %= BITS_PER_BYTE;
    return ((*m >> n) & 1);
}

#ifdef DEBUG
/*
 * This routine print the bitmap on the current console. It is used only for
 * debug purpose. If a page i is allocated, the bit i is equal to "*", otherwise
 * it is equal to"_".
 */
static void print_bitmap(void) {
    int i;
    char buf[NR_PAGES+2];

    printk("MEM_MAP: ");
    for (i=0; i<NR_PAGES; i++)
        buf[i] = get_bit(i) ? '*' : '_';
    buf[NR_PAGES] = '\n';
    buf[NR_PAGES+1] = '\0';
    printk(buf);
}
#endif

/*
 * This routine allocate a page.
 */
void* get_free_page(void) {
    int i;

    // look through a bitmap to find an empty page. We are using a first fit
    // logic.
    for (i=0; i<NR_PAGES; ++i) {
        if (get_bit(i) == 0) {
            set_bit(i);
#ifdef DEBUG
            printk("mm: page allocated %d\n", i);
            print_bitmap();
#endif
            return (void*)(MEM_INIT + i*PAGE_SIZE);
        }
    }
    panic("mm: out of memory");
    return NULL;
}

/*
 * This routine free a page.
 */
void free_page(void* page) {
    unsigned long addr = (unsigned long)page;

    // check if we are freeing a bad page
    if (addr < MEM_INIT)
        goto bad_page;
    addr -= MEM_INIT;
    if (addr%PAGE_SIZE)
        goto bad_page;
    addr /= PAGE_SIZE;
    if (addr >= NR_PAGES)
        goto bad_page;

    // clear the relative bit on the bitmap
    clear_bit(addr);

#ifdef DEBUG
    printk("mm: page freed %d\n", addr);
    print_bitmap();
#endif

    return;

bad_page:
    panic("mm: trying to free bad page!");
}
