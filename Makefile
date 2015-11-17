AS=as
LD=ld
CC=gcc
CPP=gcc -E

all: kernel bootsect setup ./tools/build image

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

kernel: head.o main.o
	$(LD) -e stext -Ttext 0x1000 -s --oformat binary head.o main.o -o $@

head.o: head.s
	$(AS) -o $@ $<

head.s: head.S
	$(CPP) -traditional $< -o $@

main.o: main.c
	$(CC) -Wall -O -fstrength-reduce -fomit-frame-pointer -c $< -o  $@

disk: image
	dd if=image of=/dev/fd0 bs=512

image: bootsect setup kernel ./tools/build
	./tools/build bootsect setup kernel > image

./tools/build: ./tools/build.c
	$(CC) -o ./tools/build ./tools/build.c

clean:
	rm -f *.o *.s
	rm bootsect
	rm setup
	rm kernel
	rm image
	rm ./tools/build
