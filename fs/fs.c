/*
 * fs.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <gollumix/kernel.h>
#include <gollumix/types.h>
#include <gollumix/stddef.h>
#include <gollumix/limits.h>
#include <gollumix/sched.h>
#include <gollumix/errno.h>
#include <asm/string.h>
#include <asm/uaccess.h>

struct file file_table[NR_FILE];

struct file_operations * chrdev_fops[MAX_CHRDEV] = {
    NULL,
};

struct file_operations * blkdev_fops[MAX_BLKDEV] = {
    NULL,
};

static int open_device(const char *pathname, struct file *filp);

asmlinkage int sys_open(const char *pathname) {
    char kpath[PATH_MAX];
    struct file * f;
    int fd, i;

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

    // In a true kernel open_device should be replaced by a namei routine that
    // convert the pathname in inode. The inode will contains the file 
	// operation for the specific file or device.
    if ((i = open_device(kpath, f)) < 0) {
        current->filp[fd]=NULL;
        f->f_count=0;
        return i;
    }

    //f->f_count = 1;
    f->f_pos = 0;
    //f->f_op = ..... inode ops ...;
    //current->filp[fd] = f;

    if (f->f_op && f->f_op->open) {
        if ((i = f->f_op->open(f))) {
            f->f_count = 0;
            current->filp[fd]=NULL;
            return i;
        }
    }

    return fd;
}

asmlinkage int sys_close(int fd) {
    struct file *filp;

    // check file descriptor
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
        return 0;
    }

    // if two processes share the same file, remove a link and exit
    if (filp->f_count > 1) {
        filp->f_count--;
        return 0;
    }

    // ok, last process has closed the file. Release resources.
    if (filp->f_op && filp->f_op->release) {
        filp->f_op->release(filp);
    }

    filp->f_count--;

    return 0;
}

asmlinkage ssize_t sys_read(int fd, char *buf, size_t count) {

    struct file *file;

    if (fd < 0 || fd >=NR_OPEN || !(current->filp[fd])) {
        return -EBADF;
    }

    file = current->filp[fd];

    if (!count) {
        return 0;
    }

    if (count < 0) {
        return -EINVAL;
    }

    if (file->f_op && file->f_op->read) {
        return file->f_op->read(file, buf, count);
    }

    return -EINVAL;
}

asmlinkage ssize_t sys_write(int fd, char *buf, size_t count) {
    struct file *file;

    if (fd < 0 || fd >=NR_OPEN) {
        return -EBADF;
    }

    file = current->filp[fd];
				    
    if (!file) {
        return -EBADF;
    }

    if (!count) {
        return 0;
    }

    if (count < 0) {
        return -EINVAL;
    }

    if (file->f_op && file->f_op->write) {
        return file->f_op->write(file, buf, count);
    }

    return -EINVAL;
}

static int open_device(const char *pathname, struct file *filp) {
    if (!strcmp(pathname, "/dev/tty0")) {
        filp->dev = 1;
        filp->f_op = chrdev_fops[0];
    } else if (!strcmp(pathname, "/dev/tty1")) {
        filp->dev = 2;
        filp->f_op = chrdev_fops[0];
    } else if (!strcmp(pathname, "/dev/tty2")) {
        filp->dev = 3;
        filp->f_op = chrdev_fops[0];
    } else if (!strcmp(pathname, "/dev/tty3")) {
        filp->dev = 4;
        filp->f_op = chrdev_fops[0];
    } else if (!strcmp(pathname, "/dev/tty4")) {
        filp->dev = 5;
        filp->f_op = chrdev_fops[0];
    } else if (!strcmp(pathname, "/dev/tty5")) {
        filp->dev = 6;
        filp->f_op = chrdev_fops[0];
    } else if (!strcmp(pathname, "/dev/ttyS0")) {
        filp->dev = 7;
        filp->f_op = chrdev_fops[0];
    } else if (!strcmp(pathname, "/dev/ttyS1")) {
        filp->dev = 8;
        filp->f_op = chrdev_fops[0];
    } else if (!strcmp(pathname, "/dev/fd0")) {
        filp->dev = 9;
        filp->f_op = blkdev_fops[0];
    } else {
        return -EINVAL;
    }

    return 0;
}
