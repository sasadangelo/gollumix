#
# drivers/chr_dev
#

OBJ=tty.o keyboard.o console.o serial.o tty_queue.o

include $(RULES)

all: chr_dev.o

chr_dev.o: $(OBJ)
	$(LD) -r -o chr_dev.o $(OBJ)

local_clean:
	rm -f *.o
