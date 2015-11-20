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
_syscall3(ssize_t, read,  int, fd, char *, buf, size_t, count)
_syscall3(ssize_t, write, int, fd, char *, buf, size_t, count)

int main(void) {
    int fd, res;
    char ch;

    print("PRG1: the process PRG1 is running.\n");

    fd = open("/dev/tty0");

    if (fd < 0) {
        print("PRG1: cannot open the tty0 terminal.\n");
    }

    char output[2];

    output[1] = '\0';

    for (;;) {
        // sleep the process
        res = read(fd, &ch, sizeof(char));

        if (res < 0) {
            print("PRG1: cannot read data from tty0.\n");
            continue;
        }

        output[0] = ch;
	
        print(output);
    }

    res = close(fd);

    if (res < 0) {
        print("PRG1: cannot close tty0.\n");
    }

    for(;;);
    return 0;
}
