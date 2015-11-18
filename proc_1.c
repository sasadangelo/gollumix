int main(void);

void _start(void) {  // offset 0x0
    while(1)
        main();
}

#include "unistd.h"

_syscall1(int, print, char*, msg)

int main(void) {
    print("PRG1: the PRG1 is running!\n");
    for(;;);
    return 0;
}
