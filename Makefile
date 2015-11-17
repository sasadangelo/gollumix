AS=as
LD=ld
CPP=gcc -E

all: kernel bootsect setup image

bootsect: bootsect.o
	$(LD) -Ttext 0x0 -s --oformat binary -o $@ $<

bootsect.o: bootsect.s
	(echo -n "SYSSIZE = ("; echo -n `ls -gG kernel | cut -c16-24`; \
		echo "+ 15 ) / 16") > tmp.s
	cat bootsect.s >> tmp.s
	mv tmp.s bootsect.s
	$(AS) -o $@ $<

bootsect.s: bootsect.S
	$(CPP) -traditional $< -o $@

setup: setup.o
	$(LD) -Ttext 0x0 -s --oformat binary -o $@ $<

setup.o: setup.s
	$(AS) -o $@ $<

setup.s: setup.S
	$(CPP) -traditional $< -o $@

kernel: main.o
	$(LD) -e stext -Ttext 0x1000 -s --oformat binary main.o -o $@

main.o: main.c
	$(CC) -Wall -O -fstrength-reduce -fomit-frame-pointer -c $< -o  $@

disk: image
	dd if=image of=/dev/fd0 bs=512

image: bootsect setup kernel
	cat bootsect > image
	cat setup >> image
	cat kernel >> image

clean:
	rm *.o
	rm *.s
	rm image
	rm bootsect
	rm setup
	rm kernel
