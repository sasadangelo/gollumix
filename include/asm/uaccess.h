#ifndef UACCESS_H
#define UACCESS_H

static inline unsigned char get_user_byte(const char * addr) {
    register unsigned char _v;

    __asm__ ("movb %%fs:%1,%0":"=q" (_v):"m" (*addr));
    return _v;
}

extern inline void put_user_byte(char val,char *addr) {
    __asm__ ("movb %0,%%fs:%1"::"q" (val),"m" (*addr));
}

extern long strncpy_from_user(char *dst, const char *src, long count);

extern long copy_to_user(char *dst, const char *src, long count);

#endif
