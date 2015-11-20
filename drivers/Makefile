#
# drivers/Makefile
#

OBJS= chr_dev/chr_dev.o
SUB_DIRS=chr_dev

include $(RULES)

all: drivers.o

drivers.o: sub_dirs
	$(LD) -r -o drivers.o $(OBJS)

local_clean:
	rm -f *.o

