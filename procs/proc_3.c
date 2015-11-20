#include <gollumix/unistd.h>

int main(void) {
    int fd, res;
    char ch;

    print("PRG3: the process PRG3 is running.\n");

    fd = open("/dev/tty2");

    if (fd < 0) {
        print("PRG3: cannot open the tty2 terminal.\n");
        return 0;
    }

    for (;;) {
        // sleep the process
        res = read(fd, &ch, sizeof(char));

        if (res < 0) {
            print("PRG3: cannot read data from tty2.\n");
            continue;
        }

        res = write(fd, &ch, sizeof(char));
        
		if (res < 0) {
            print("PRG3: cannot write data to tty2.\n");
            continue;
        }
    }

    res = close(fd);

    if (res < 0) {
        print("PRG3: cannot close tty2.\n");
        return 0;
    }

    return 0;
}
