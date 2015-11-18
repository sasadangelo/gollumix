#ifndef UACCESS_H
#define UACCESS_H

extern inline unsigned char get_fs_byte(const char *addr) {
    unsigned register char _v;

    __asm__ __volatile__("movb %%fs:%1,%0":"=r" (_v):"m" (*addr));
    return _v;
}

#endif
