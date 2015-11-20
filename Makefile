AS=as
LD=ld 
CC=gcc
CPP=gcc -E
LDS= kernel.lds

#Select one of these two option if you want compile in debug or not debug mode
DEBUG =
#DEBUG = -DDEBUG

# Please select your keyboard map
#KEYBOARD = -DKBD_US
KEYBOARD = -DKBD_IT

KERNEL_OBJ=head.o main.o tty.o keyboard.o console.o asm.o vsprintf.o irq.o \
	traps.o time.o mktime.o sched.o sys.o mm.o panic.o errno.o fork.o exec.o \
	usercopy.o serial.o fs.o tty_queue.o task.o wait_queue.o

all: image

bootsect: bootsect.o
	$(LD) -Ttext 0x0 -s --oformat binary -o $@ $<

bootsect.o: bootsect.s
	$(AS) -o $@ $<

%.s: %.S
	$(CPP) -traditional $< -o $@

setup: setup.o
	$(LD) -Ttext 0x0 -s --oformat binary -o $@ $<

setup.o: setup.s
	$(AS) -o $@ $<

kernel: $(KERNEL_OBJ) $(LDS)
	$(LD) -e stext -T $(LDS) -s --oformat binary $(KERNEL_OBJ) -o $@

$(LDS):
	$(CPP) -nostdinc -P -C -Ui386 -o $@ $@.S

head.o: head.s
	$(AS) -o $@ $<

.c.o:
	$(CC) $(KEYBOARD) $(DEBUG) -I. -nostdinc -Wall -O -fomit-frame-pointer -c $< -o  $@

asm.o: asm.S
	gcc -c -o $@ $<

disk: image
	dd if=image of=/dev/fd0 bs=512

image: kernel bootsect setup ./tools/build proc_1.bin
	./tools/build bootsect setup kernel 1 proc_1.bin > image

./tools/build: ./tools/build.c
	$(CC) -o ./tools/build ./tools/build.c

%.bin: %.o errno.o syscall.o
	$(LD) -T proc.lds -o $@ $< errno.o syscall.o

clean:
	rm -f *.o *.s kernel.lds *.bin
	rm -rf bootsect setup kernel image ./tools/build
