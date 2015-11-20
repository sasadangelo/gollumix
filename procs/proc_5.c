int main(void);

void _start(void) {  // offset 0x0
    main();
    for(;;);
}

#include "unistd.h"

_syscall1(int, print, char*, msg)
_syscall0(int, fork)
_syscall1(int, open, const char*, pathname)
_syscall1(int, close, int, fd)
_syscall3(ssize_t, read,  int, fd, char *, buf, size_t, count)
_syscall3(ssize_t, write, int, fd, char *, buf, size_t, count)

static int tty_COM1() {
    int  fd_rs, fd_tty, res;
    char ch;

    fd_tty = open("/dev/tty4");

    if (fd_tty < 0) {
        print("PRG5: cannot open the tty4 terminal.\n");
        return 0;
    }

    fd_rs = open("/dev/ttyS0");

    if (fd_tty < 0) {
        print("PRG5: cannot open the ttyS0 serial line.\n");
        return 0;
    }

    for (;;) {
        // wait for characters typed by keybord ...
        res = read(fd_tty, &ch, sizeof(char));

        if (res < 0) {
            print("PRG5: cannot read data from tty4.\n");
            continue;
        }

        // write character on serial line ttyS0
        res = write(fd_rs, &ch, sizeof(char));

        if (res < 0) {
            print("PRG5: cannot write data to ttyS0.\n");
            continue;
        }
    }

    res = close(fd_tty);

    if (res < 0) {
        print("PRG5: cannot close tty4.\n");
        return 0;
    }

    res = close(fd_rs);

    if (res < 0) {
        print("PRG5: cannot close ttyS0.\n");
        return 0;
    }
}

static int COM1_tty() {
    int  fd_rs, fd_tty, res;
    char ch;

    fd_tty = open("/dev/tty4");
    
	if (fd_tty < 0) {
        print("PRG5: cannot open the tty4 terminal.\n");
        return 0;
    }
    
	fd_rs = open("/dev/ttyS0");
    
	if (fd_rs < 0) {
        print("PRG5: cannot open the ttyS0 serial line.\n");
        return 0;
    }

    for (;;) {
        // wait for characters coming from COM1 ...
        res = read(fd_rs, &ch, sizeof(char));

		if (res < 0) {
            print("PRG5: cannot read data from ttyS0.\n");
            continue;
        }
		
		// write character to tty4
        res = write(fd_tty, &ch, sizeof(char));

        if (res < 0) {
            print("PRG5: cannot write data to tty.\n");
            continue;
        }

    }

    res = close(fd_tty);
	    
    if (res < 0) {
        print("PRG5: cannot close tty4.\n");
        return 0;
    }

    res = close(fd_rs);

    if (res < 0) {
        print("PRG5: cannot close ttyS0.\n");
        return 0;
    }
}

int main(void) {
    int pid;

    print("PRG5: the process PRG5 is running.\n");

    pid = fork();

    if (pid == 0) {
        // child process: it read from COM1 and write on tty4
        print("PRG5 child: the process PRG5 child is running.\n");
        return COM1_tty();
    } else if (pid > 0) {
        // parent process: it read from tty4 and write on COM1
        return tty_COM1();
    } else {
        print("PRG5: cannot fork child 1.\n");
        return 0;
    }

    return 0;
}
