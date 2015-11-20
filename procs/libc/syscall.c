extern int main(void);

void _start(void) {  // offset 0x0
    main();
    while(1);
}

#include <gollumix/unistd.h>

// the signature of the supported system call
//_syscall1(int, print, char*, msg)
_syscall0(int, fork)
_syscall1(int, exec, char *, filename)
_syscall1(int, open, const char*, pathname)
_syscall1(int, close, int, fd)
_syscall3(ssize_t, read,  int, fd, char *, buf, size_t, count)
_syscall3(ssize_t, write, int, fd, char *, buf, size_t, count)
//_syscall1(int, print_hex, unsigned char, value)

