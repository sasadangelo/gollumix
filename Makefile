AS=as
LD=ld
CPP=gcc -E

all: bootsect setup image

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

disk: image
	dd if=image of=/dev/fd0 bs=512

image: bootsect setup
	cat bootsect > image
	cat setup >> image

clean:
	rm *.o
	rm *.s
	rm image
	rm bootsect
	rm setup
