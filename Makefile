AS=as
LD=ld 
CC=gcc
CPP=gcc -E

#Select one of these two option if you want compile in debug or not debug mode
#DEBUG =
DEBUG = -DDEBUG

# Please select your keyboard map
#KEYBOARD = -DKBD_US
KEYBOARD = -DKBD_IT

KERNEL_OBJ=head.o main.o tty_io.o keyboard.o console.o asm.o vsprintf.o irq.o \
	traps.o time.o mktime.o sched.o sys.o errno.o mm.o panic.o

all: image

bootsect: bootsect.o
	$(LD) -Ttext 0x0 -s --oformat binary -o $@ $<

bootsect.o: bootsect.s
	$(AS) -o $@ $<

bootsect.s: bootsect.S
	$(CPP) -traditional $< -o $@

setup: setup.o
	$(LD) -Ttext 0x0 -s --oformat binary -o $@ $<

setup.o: setup.s
	$(AS) -o $@ $<

setup.s: setup.S
	$(CPP) -traditional $< -o $@

kernel: $(KERNEL_OBJ)
	$(LD) -e stext -Ttext 0x1000 -s --oformat binary $(KERNEL_OBJ) -o $@

head.o: head.s
	$(AS) -o $@ $<

head.s: head.S
	$(CPP) -traditional $< -o $@

.c.o:
	$(CC) $(KEYBOARD) $(DEBUG) -I. -nostdinc -Wall -O -fomit-frame-pointer -c $< -o  $@

asm.o: asm.S
	gcc -c -o $@ $<

disk: image
	dd if=image of=/dev/fd0 bs=512

image: kernel bootsect setup ./tools/build
	./tools/build bootsect setup kernel > image

./tools/build: ./tools/build.c
	$(CC) -o ./tools/build ./tools/build.c

clean:
	rm -f *.o *.s
	rm -rf bootsect setup kernel image ./tools/build
