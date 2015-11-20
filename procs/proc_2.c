#include <gollumix/unistd.h>

int main(void) {
    int fd, res;
    char ch;

    print("PRG2: the process PRG2 is running.\n");

    fd = open("/dev/tty1");

    if (fd < 0) {
        print("PRG2: cannot open the tty1 terminal.\n");
        return 0;
    }

    for (;;) {
        // sleep the process
        res = read(fd, &ch, sizeof(char));

        if (res < 0) {
            print("PRG2: cannot read data from tty1.\n");
            continue;
        }

        res = write(fd, &ch, sizeof(char));
        
		if (res < 0) {
            print("PRG2: cannot write data to tty1.\n");
            continue;
        }
    }

    res = close(fd);

    if (res < 0) {
        print("PRG2: cannot close tty1.\n");
        return 0;
    }

    return 0;
}
