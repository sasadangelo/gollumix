#include <gollumix/unistd.h>

extern int printf(const char *fmt, ...);

int main(void) {
    int fd, res;
    char ch;

    fd = open("/dev/tty2");

    if (fd < 0) {
        return -1;
    }

    printf("PRG3: the process PRG3 is running.\n");

    for (;;) {
        // sleep the process
        res = read(fd, &ch, sizeof(char));

        if (res < 0) {
            printf("PRG3: cannot read data from tty2.\n");
            continue;
        }

        res = write(fd, &ch, sizeof(char));
        
		if (res < 0) {
            printf("PRG3: cannot write data to tty2.\n");
            continue;
        }
    }

    res = close(fd);

    if (res < 0) {
        return -2;
    }

    return 0;
}
