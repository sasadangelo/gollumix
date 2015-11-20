#
# boot/Makefile
#

SUB_DIRS=libc
LDS=proc.lds

include $(RULES)

all: proc_1.bin proc_2.bin proc_3.bin proc_4.bin proc_5.bin proc_6.bin


%.bin: %.o libc/libc.o
	@echo "[LD] $@"
	@$(LD) -T $(LDS) -o $@ libc/libc.o $<

local_clean:
	@rm -f *.bin *.o
