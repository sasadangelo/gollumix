#
# procs/libc/Makefile
#

OBJS = syscall.o errno.o printf.o

include $(RULES)

all: libc.o

libc.o: $(OBJS)
	@echo "[LD] $@"
	$(LD) -r -o $@ $(OBJS)

local_clean:
	@rm -f *.o	
