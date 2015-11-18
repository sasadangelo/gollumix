#include "unistd.h"

// the signature of the supported system call
_syscall1(int, print, char *, msg);
_syscall0(int, fork);
_syscall1(int, exec, char *, filename);
