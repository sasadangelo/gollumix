AS=as
LD=ld

all: bootsect image

bootsect: bootsect.o
	$(LD) -Ttext 0x0 -s --oformat binary -o $@ $<

bootsect.o: bootsect.S
	$(AS) -o $@ $<

disk: image
	dd if=image of=/dev/fd0 bs=512

image: bootsect
	cat bootsect > image

clean:
	rm *.o
	rm image
	rm bootsect
