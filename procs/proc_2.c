#include <gollumix/unistd.h>

extern int printf(const char *fmt, ...);

int main(void) {
    int fd, res;
    char ch;


    fd = open("/dev/tty1");

    if (fd < 0) {
        return -1;
    }

    printf("PRG2: the process PRG2 is running.\n");

    for (;;) {
        // sleep the process
        res = read(fd, &ch, sizeof(char));

        if (res < 0) {
            printf("PRG2: cannot read data from tty1.\n");
            continue;
        }

        res = write(fd, &ch, sizeof(char));
        
		if (res < 0) {
            printf("PRG2: cannot write data to tty1.\n");
            continue;
        }
    }

    res = close(fd);

    if (res < 0) {
        return -2;
    }

    return 0;
}
