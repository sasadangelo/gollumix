int main(void);

void _start(void) {  // offset 0x0
    while(1)
        main();
}

#include "unistd.h"

_syscall1(int, print, char*, msg)
_syscall0(int, fork)

int main(void) {
    int pid;

    pid = fork();

    if (pid == 0) {
        print("PRG1: the process PRG1 child is running\n");
        for(;;); 
    } else if (pid < 0) {
        print("PRG1: the process PRG1 cannot be forked\n");
    }

    print("PRG1: the process PRG1 is running\n");
    for(;;);
    return 0;
}
