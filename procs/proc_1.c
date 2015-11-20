#include <gollumix/unistd.h>

int main(void) {
    int fd, res;
    char ch;

    print("PRG1: the process PRG1 is running.\n");

    fd = open("/dev/tty0");

    if (fd < 0) {
        print("PRG1: cannot open the tty0 terminal.\n");
        return 0;
    }

    for (;;) {
        // sleep the process
        res = read(fd, &ch, sizeof(char));

        if (res < 0) {
            print("PRG1: cannot read data from tty0.\n");
            continue;
        }

        res = write(fd, &ch, sizeof(char));

        if (res < 0) {
            print("PRG1: cannot write data to tty0.\n");
            continue;
        }
    }

    res = close(fd);

    if (res < 0) {
        print("PRG1: cannot close tty0.\n");
        return 0;
    }

    return 0;
}
