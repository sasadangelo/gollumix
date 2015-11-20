#include <gollumix/unistd.h>

int main(void) {
    int fd, res;
    char ch;

    print("PRG4: the process PRG4 is running.\n");

    fd = open("/dev/tty3");

    if (fd < 0) {
        print("PRG4: cannot open the tty3 terminal.\n");
        return 0;
    }

    for (;;) {
        // sleep the process
        res = read(fd, &ch, sizeof(char));

        if (res < 0) {
            print("PRG4: cannot read data from tty3.\n");
            continue;
        }

        res = write(fd, &ch, sizeof(char));
        
		if (res < 0) {
            print("PRG4: cannot write data to tty3.\n");
            continue;
        }
    }

    res = close(fd);

    if (res < 0) {
        print("PRG4: cannot close tty3.\n");
        return 0;
    }

    return 0;
}
