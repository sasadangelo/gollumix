#
# kernel/Makefile
#

OBJCOPY=objcopy -O binary -R .note -R .comment -S
LDS = kernel.lds
MAPFILE = System.map

# Object files to link to the kernel code
FILESYSTEM=$(TOPDIR)/fs/fs.o
DRIVERS=$(TOPDIR)/drivers/drivers.o

#OBJ=head.o main.o tty.o keyboard.o console.o asm.o vsprintf.o irq.o \
#	traps.o time.o mktime.o sched.o sys.o mm.o panic.o errno.o fork.o exec.o \
#	usercopy.o serial.o tty_queue.o task.o wait_queue.o

OBJ=head.o main.o asm.o vsprintf.o irq.o traps.o time.o mktime.o sched.o \
	sys.o mm.o panic.o errno.o fork.o exec.o usercopy.o task.o wait_queue.o

include $(RULES)

all: kernel

kernel: kernel.elf
	@echo "[OBJCOPY] $@"
	@$(OBJCOPY) $< $@

kernel.elf: $(OBJ) $(LDS)
	@echo "[LD] $@"
	@$(LD) -e stext -T $(LDS) -o $@ $(OBJ) $(FILESYSTEM) $(DRIVERS)
	@nm $@ | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aU] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | sort > $(MAPFILE)

$(LDS): $(LDS).S
	@echo "[CPP] $@"
	@$(CPP) $(CPPFLAGS) -nostdinc -P -C -Ui386 -o $@ $@.S

asm.o: asm.S
	@echo "[CC] $@"
	@gcc $(CPPFLAGS) -c -o $@ $<

head.s: head.S
	@echo "[CC] $@"
	@$(CPP) $(CPPFLAGS) -traditional $< -o $@

local_clean:
	rm -f $(LDS) $(MAPFILE) *.o *.s kernel kernel.elf

