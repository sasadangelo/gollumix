TOPDIR=$(CURDIR)
DIRNAME =`basename $(TOPDIR)`
FUNNYDIR=
RULES=$(TOPDIR)/Rules.make

AS=as
LD=ld 
CC=gcc
CPP=gcc -E
MAKE=make -s

# Please select your keyboard map
#KEYBOARD = -DKBD_US
KEYBOARD = -DKBD_IT

#Select one of these two option if you want compile in debug or not debug mode
DEBUG =
#DEBUG = -DDEBUG

CPPFLAGS=-I$(TOPDIR)/include
CFLAGS=$(CPPFLAGS) $(KEYBOARD) $(DEBUG) -c -Wall -Wstrict-prototypes -I. -nostdinc -O -fomit-frame-pointer -fno-common -fno-builtin

export TOPDIR FUNNYDIR RULES LD CC CPP MAKE KEYBOARD CPPFLAGS CFLAGS

SUB_DIRS=boot fs drivers kernel procs tools

.PHONY: gollumix image

include $(RULES)

all: image

disk: do_it_all
	@echo "[DISK]"
	@dd if=image of=/dev/fd0 bs=512

image: kernel/kernel
	@echo "[KERNEL IMAGE]"
	@./tools/build boot/bootsect boot/setup kernel/kernel 6 procs/proc_?.bin

local_clean:
	@rm -f image
