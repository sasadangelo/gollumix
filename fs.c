#include "kernel.h"
#include "types.h"
#include "stddef.h"
#include "limits.h"

asmlinkage int sys_open(const char *pathname) {
    char *kpath[PATH_MAX];

    strncpy_from_user(kpath, pathname, PATH_MAX);
    printk("open file %s.\n", kpath);
    return 1;
}

asmlinkage int sys_close(int fd) {
    printk("close file %d.\n", fd);
    return 0;
}

asmlinkage ssize_t sys_read(int fd, void *buf, size_t count) {
    printk("read from file %d.\n", fd);
    return 0;
}

asmlinkage ssize_t sys_write(int fd, void *buf, size_t count) {
    printk("write to file %d.\n", fd);
    return 0;
}
