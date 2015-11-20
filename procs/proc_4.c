#include <gollumix/unistd.h>
#include <gollumix/stdarg.h>

#define BLOCK_SIZE 512
#define IMAGE_SIZE 0x20

char block[BLOCK_SIZE];

extern int printf(const char *fmt, ...);

int main(void) {
    //unsigned char display;
    int fd_fd0, fd_tty, res, i, bytes_read = 0, count = 0, bytes_reading = 0;
    char ch;

    fd_tty = open("/dev/tty3");

    if (fd_tty < 0) {
        return -1;
    }

    printf("PRG4: the process PRG4 is running.\n");
    printf("Please, press a key to start reading from floppy.\n");

    // sleep the process
    res = read(fd_tty, &ch, sizeof(char));

    if (res < 0) {
        printf("PRG4: cannot read data from tty3.\n");
        return -2;
    }

    fd_fd0 = open("/dev/fd0");

    if (fd_fd0 < 0) {
        printf("PRG4: cannot open the floppy driver.\n");
        return -3;
    }

    bytes_reading = IMAGE_SIZE;

    while(bytes_reading > 0) {
        if (bytes_reading > BLOCK_SIZE) {
            count = read(fd_fd0, block, BLOCK_SIZE);
        } else {
            count = read(fd_fd0, block, bytes_reading);
        }

        if (count < 0) {
            printf("PRG4: error reading from floppy driver.\n");
            return -4;
        }

        for (i=0; i<count; ++i) {
            printf("%x - ", (unsigned char) block[i]);
        }

        printf("\n");

        bytes_read += count;
        bytes_reading -= count;
    } 

    res = close(fd_fd0);

    if (res < 0) {
        printf("PRG4: cannot close floppy driver.\n");
        return -5;
    }

    res = close(fd_tty);

    if (res < 0) {
        return -6;
    }

    return 0;
}
