#ifndef BLKDEV_H
#define BLKDEV_H

//#define MAX_BLKDEV 10

struct blkdev {
    // next block device in the Block Device List.
    struct blkdev *next;
    // Hardcoded name of the driver
    char *name;
    // sector size
    int sectsize;
    // sector size bits shift
    int sectbits;
    // major,min numbers
    unsigned char major;
    unsigned char minor;
    // each block device should implement the following operations.
    struct file_operations *blkdev_fops; 
    // read/write routine stub
    //int (*ll_rw_blk)(int, block_t, char *, size_t);
    // Can be frobbed with
    //size_t count;
};

// Block Device Management Interface
void   blkdev_add(struct blkdev *blk);
struct blkdev *blkdev_get(char *name);

#endif
