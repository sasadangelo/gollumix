#ifndef FS_H
#define FS_H

#include <gollumix/types.h>

// only 1 char driver: tty
// only 1 block driver: floppy
#define MAX_CHRDEV 1
#define MAX_BLKDEV 1

// number of open files
#define NR_OPEN    20
#define NR_FILE    64

struct file {
    dev_t dev;                     // the device
    off_t          f_pos;          // file pointer position
    unsigned short f_count;        // file use count
    struct file_operations * f_op; // file operations
};


struct file_operations {
    int  (*open)    (struct file *);
    int  (*read)    (struct file *, char *, int);
    int  (*write)   (struct file *, char *, int);
    void (*release) (struct file *);
};

extern struct file_operations * chrdev_fops[MAX_CHRDEV];
extern struct file_operations * blkdev_fops[MAX_BLKDEV];

#endif
