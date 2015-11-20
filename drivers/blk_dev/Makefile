#
# drivers/chr_dev
#

OBJ= dma.o floppy.o blkdev.o

include $(RULES)

all: blk_dev.o

blk_dev.o: $(OBJ)
	@echo "[LD] $@"
	@$(LD) -r -o blk_dev.o $(OBJ)

local_clean:
	rm -f *.o
