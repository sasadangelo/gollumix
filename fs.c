#include "kernel.h"
#include "types.h"
#include "stddef.h"
#include "limits.h"
#include "sched.h"
#include "errno.h"
#include "string.h"
#include "uaccess.h"

struct file file_table[NR_FILE];

struct file_operations * chrdev_fops[MAX_CHRDEV] = {
    NULL,
};

static int open_tty(const char *pathname, struct file *filp);

asmlinkage int sys_open(const char *pathname) {
    char kpath[PATH_MAX];
    struct file * f;
    int fd, i;

    printk("open: start.\n");

    // look for a free file descriptor
    for(fd=0 ; fd<NR_OPEN ; fd++) {
        if (!current->filp[fd]) {
            break;
        }
    }

    if (fd>=NR_OPEN) {
        // no file descriptor available, return an error
        return -EINVAL;
    }

    printk("fd=%d, look for a file.\n", fd);

    // look for a free file in the file table
    f=0+file_table;

    for (i=0 ; i<NR_FILE ; i++,f++) {
        if (!f->f_count) {
            break;
        }
    }

	if (i>=NR_FILE) {
        // no file available, return an error
        return -EINVAL;
    }

    // assign the new file to the current process and try to open the tty
    (current->filp[fd] = f)->f_count++;

    // copy pathname from user space to kernel space
    strncpy_from_user(kpath, pathname, PATH_MAX);

    // In a true kernel open_tty should be replaced by a namei routine that
    // convert the pathname in inode. The inode will contains the file 
	// operation for the specific file or device.
    if ((i = open_tty(kpath, f)) < 0) {
        printk("open_tty: failed.\n");
        current->filp[fd]=NULL;
        f->f_count=0;
        return i;
    }

    printk("open_tty: ok.\n");
    //f->f_count = 1;
    //f->f_op = ..... inode ops ...;
    //current->filp[fd] = f;

    if (f->f_op && f->f_op->open) {
        if ((i = f->f_op->open(f))) {
	        printk("tty_open: failed.\n");
            f->f_count = 0;
            current->filp[fd]=NULL;
            return i;
        }
    }

    printk("open: successful: %d.\n", fd);
    return fd;
}

asmlinkage int sys_close(int fd) {
    struct file *filp;

    printk("close: start.\n");

    // chack file descriptor
    if (fd < 0 || fd >=NR_OPEN) { 
        return -EBADF;
    }

    // check if exists a file for the input fd
    filp = current->filp[fd];
    if (!filp) {
        return -EINVAL;
    }

    // release the file descriptor
    current->filp[fd] = NULL;

    if (filp->f_count == 0) {
        printk("close: file count is 0\n");
        return 0;
    }

    // if two processes share the same file, remove a link and exit
    if (filp->f_count > 1) {
        filp->f_count--;
        return 0;
    }

    printk("close: continue.\n");

    // ok, last process has closed the file. Release resources.
    if (filp->f_op && filp->f_op->release) {
        filp->f_op->release(filp);
    }

    filp->f_count--;

    printk("close: successful.\n");
    return 0;
}

asmlinkage ssize_t sys_read(int fd, void *buf, size_t count) {
    return -EINVAL;
}

asmlinkage ssize_t sys_write(int fd, void *buf, size_t count) {
    return -EINVAL;
}

static int open_tty(const char *pathname, struct file *filp) {
    printk("open_tty: %s start.\n", pathname);
    if (!strcmp(pathname, "/dev/tty0")) {
        filp->dev = 0;
    } else if (!strcmp(pathname, "/dev/tty1")) {
        filp->dev = 1;
    } else if (!strcmp(pathname, "/dev/tty2")) {
        filp->dev = 2;
    } else if (!strcmp(pathname, "/dev/tty3")) {
        filp->dev = 3;
    } else if (!strcmp(pathname, "/dev/ttyS0")) {
        filp->dev = 4;
    } else if (!strcmp(pathname, "/dev/ttyS1")) {
        filp->dev = 5;
    } else {
        printk("open_tty: failed.\n");
        return -EINVAL;
    }

    filp->f_op = chrdev_fops[0];
    return 0;
}
