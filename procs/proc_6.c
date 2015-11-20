#include <gollumix/unistd.h>

static int tty_COM2() {
    int  fd_rs, fd_tty, res;
    char ch;

    fd_tty = open("/dev/tty5");

    if (fd_tty < 0) {
        print("PRG6: cannot open the tty5 terminal.\n");
        return 0;
    }

    fd_rs = open("/dev/ttyS1");

    if (fd_tty < 0) {
        print("PRG6: cannot open the ttyS1 serial line.\n");
        return 0;
    }

    for (;;) {
        // wait for characters typed by keybord ...
        res = read(fd_tty, &ch, sizeof(char));

        if (res < 0) {
            print("PRG6: cannot read data from tty5.\n");
            continue;
        }

        // write character on serial line ttyS1
        res = write(fd_rs, &ch, sizeof(char));
        
        if (res < 0) {
            print("PRG6: cannot write data to ttyS1.\n");
            continue;
        }
    }

    res = close(fd_tty);

    if (res < 0) {
        print("PRG6: cannot close tty5.\n");
        return 0;
    }

    res = close(fd_rs);

    if (res < 0) {
        print("PRG6: cannot close ttyS1.\n");
        return 0;
    }
}

static int COM2_tty() {
    int  fd_rs, fd_tty, res;
    char ch;

    fd_tty = open("/dev/tty5");
    
	if (fd_tty < 0) {
        print("PRG6: cannot open the tty5 terminal.\n");
        return 0;
    }
    
	fd_rs = open("/dev/ttyS1");
    
	if (fd_rs < 0) {
        print("PRG6: cannot open the ttyS1 serial line.\n");
        return 0;
    }

    for (;;) {
        // wait for characters coming from COM1 ...
        res = read(fd_rs, &ch, sizeof(char));

		if (res < 0) {
            print("PRG6: cannot read data from ttyS1.\n");
            continue;
        }
		
		// write character to tty4
        res = write(fd_tty, &ch, sizeof(char));

        if (res < 0) {
            print("PRG6: cannot write data to tty.\n");
            continue;
        }

    }

    res = close(fd_tty);
	    
    if (res < 0) {
        print("PRG6: cannot close tty5.\n");
        return 0;
    }

    res = close(fd_rs);

    if (res < 0) {
        print("PRG6: cannot close ttyS1.\n");
        return 0;
    }
}

int main(void) {
    int pid;

    print("PRG6: the process PRG6 is running.\n");

    pid = fork();

    if (pid == 0) {
        // child process: it read from COM2 and write on tty5
        print("PRG6 child: the process PRG6 child is running.\n");
        return COM2_tty();
    } else if (pid > 0) {
        // parent process: it read from tty5 and write on COM2
        return tty_COM2();
    } else {
        print("PRG6: cannot fork child 1.\n");
        return 0;
    }

    return 0;
}
