#
# boot/Makefile
#

include $(RULES)

all: bootsect setup

bootsect.s: bootsect.S
	@echo "[CC] $@"
	@$(CPP) $(CPPFLAGS) -traditional $< -o $@

bootsect: bootsect.o
	@echo "[LD] $@"
	@$(LD) -Ttext 0x0 -s --oformat binary -o $@ $<

setup.s: setup.S
	@echo "[CC] $@"
	@$(CPP) -traditional $< -o $@

setup: setup.o
	@echo "[LD] $@"
	@$(LD) -Ttext 0x0 -s --oformat binary -o $@ $<

local_clean:
	@rm -rf *.s *.o bootsect setup
