#include <gollumix/unistd.h>
#include <gollumix/stdarg.h>

extern int printf(const char *fmt, ...);

int main(void) {
    int fd, res;
    char ch;

    fd = open("/dev/tty0");

    if (fd < 0) {
        return -1;
    }

    printf("PRG1: the process PRG1 is running.\n");

    for (;;) {
        // sleep the process
        res = read(fd, &ch, sizeof(char));

        if (res < 0) {
            printf("PRG1: cannot read data from tty0.\n");
            continue;
        }

        res = write(fd, &ch, sizeof(char));

        if (res < 0) {
            printf("PRG1: cannot write data to tty0.\n");
            continue;
        }
    }

    res = close(fd);

    if (res < 0) {
        return -2;
    }

    return 0;
}
