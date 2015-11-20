int main(void);

void _start(void) {  // offset 0x0
    while(1)
        main();
}

#include "unistd.h"

_syscall1(int, print, char*, msg)
_syscall0(int, fork)
_syscall1(int, open, const char*, pathname)
_syscall1(int, close, int, fd)
_syscall3(ssize_t, read, int, fd, void *, buf, size_t, count)
_syscall3(ssize_t, write, int, fd, void *, buf, size_t, count)

int main(void) {
    int fd;
    
    print("PRG1: the process PRG1 is running\n");

    fd = open("/dev/tty0");
    close(fd);
    for(;;);
    return 0;
}
