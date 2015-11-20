#ifndef FS_H
#define FS_H

#include "types.h"

// only 1 char driver: tty
#define MAX_CHRDEV 1

// number of open files
#define NR_OPEN    20
#define NR_FILE    64

struct file {
    dev_t dev;                     // the device
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

#endif
