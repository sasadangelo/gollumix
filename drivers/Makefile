#
# drivers/Makefile
#

OBJS= chr_dev/chr_dev.o blk_dev/blk_dev.o
SUB_DIRS=chr_dev blk_dev

include $(RULES)

all: drivers.o

#drivers.o: sub_dirs
drivers.o: $(OBJS)
	@echo "[LD] $@"
	@$(LD) -r -o drivers.o $(OBJS)

local_clean:
	rm -f *.o

