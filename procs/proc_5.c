#include <gollumix/unistd.h>

extern int printf(const char *fmt, ...);

static int fd_tty;

static int tty_COM1(void) {
    int  fd_rs, res;
    char ch;

    fd_rs = open("/dev/ttyS0");

    if (fd_rs < 0) {
        printf("PRG5: cannot open the ttyS0 serial line.\n");
        return -1;
    }

    for (;;) {
        // wait for characters typed by keybord ...
        res = read(fd_tty, &ch, sizeof(char));

        if (res < 0) {
            printf("PRG5: cannot read data from tty4.\n");
            continue;
        }

        // write character on serial line ttyS0
        res = write(fd_rs, &ch, sizeof(char));

        if (res < 0) {
            printf("PRG5: cannot write data to ttyS0.\n");
            continue;
        }
    }

    res = close(fd_rs);

    if (res < 0) {
        printf("PRG5: cannot close ttyS0.\n");
        return -2;
    }

    return 0;
}

static int COM1_tty(void) {
    int  fd_rs, res;
    char ch;

	fd_rs = open("/dev/ttyS0");
    
	if (fd_rs < 0) {
        printf("PRG5: cannot open the ttyS0 serial line.\n");
        return -3;
    }

    for (;;) {
        // wait for characters coming from COM1 ...
        res = read(fd_rs, &ch, sizeof(char));

		if (res < 0) {
            printf("PRG5: cannot read data from ttyS0.\n");
            continue;
        }
		
		// write character to tty4
        res = write(fd_tty, &ch, sizeof(char));

        if (res < 0) {
            printf("PRG5: cannot write data to tty.\n");
            continue;
        }
    }

    res = close(fd_rs);

    if (res < 0) {
        printf("PRG5: cannot close ttyS0.\n");
        return -4;
    }

    return 0;
}

int main(void) {
    int pid, res;

    fd_tty = open("/dev/tty4");

    if (fd_tty < 0) {
        return -5;
    }


    printf("PRG5: the process PRG5 is running.\n");

    pid = fork();

    if (pid == 0) {
        // child process: it read from COM1 and write on tty4
        printf("PRG5 child: the process PRG5 child is running.\n");
        return COM1_tty();
    } else if (pid > 0) {
        // parent process: it read from tty4 and write on COM1
        return tty_COM1();
    } else {
        printf("PRG5: cannot fork child 1.\n");
        return 0;
    }

    res = close(fd_tty);

    if (res < 0) {
        return -6;
    }

    return 0;
}
