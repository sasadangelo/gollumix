BOOTSEG = 0x07C0
INITSEG = 0x9000

.code16
.text

.global _start

_start:
    movw $BOOTSEG, %ax
    movw %ax, %ds               # DS = 0x07C0
    movw $INITSEG, %ax
    movw %ax, %es               # ES = 0x9000
    movw $256, %cx              # CX = 256
    subw %si, %si               # SI = 0
    subw %di, %di               # DI = 0
    cld                         # copy 256 words from DS:SI to ES:DI
    rep
    movsw
    ljmp    $INITSEG, $go       # jump to 0x9000:$go

# Basically in the memory there is the following situation:
#
#    ---------------
#   |               |
#   |_______________|
#   |   BOOTSECTOR  | 0x7C0:0
#   |               |
#   |go: ....       |
#   |               |
#   |_______________|
#   |               |
#   |   .........   |
#   |_______________|
#   |   BOOTSECTOR  | 0x9000:0
#   |     (COPY)    |
#   |               |
#   |go: ....       | <- now we are here
#   |               |
#   |_______________|
#   |               | 0x9000:0x200 = 0x90200
#   |    ......     |
#   |_______________|

go:

# set the stack segment. It will start from 0x9000:0x4000-12 (in fact SS:SP
# will be set with this value) and it will grow along the top direction

    movw    $0x4000-12, %di
    movw    %ax, %ds            # %ds = 0x9000
    movw    %ax, %ss            # %ss = 0x9000
    movw    %di, %sp            # %sp = 0x4000-12

# print the "Hello World!!!" message.

    movb $0xE, %ah              # Function 0x0E of Interrupt 0x10
    movb $'H', %al              # write 'H'
    int  $0x10
    movb $'e', %al              # write 'e'
    int  $0x10
    movb $'l', %al              # write 'l'
    int  $0x10
    movb $'l', %al              # write 'l'
    int  $0x10
    movb $'o', %al              # write 'o'
    int  $0x10
    movb $' ', %al              # write ' '
    int  $0x10
    movb $'W', %al              # write 'W'
    int  $0x10
    movb $'o', %al              # write 'o'
    int  $0x10
    movb $'r', %al              # write 'r'
    int  $0x10
    movb $'l', %al              # write 'l'
    int  $0x10
    movb $'d', %al              # write 'd'
    int  $0x10
    movb $'!', %al              # write '!'
    int  $0x10
    movb $'!', %al              # write '!'
    int  $0x10
    movb $'!', %al              # write '!'
    int  $0x10
	call kill_motor				# stop the floppy motor

done:                           # infinite loop
	jmp done

# kill_motor
#
# This procedure turns off the floppy drive motor, so that we enter the kernel
# in a known state, and don't have to worry about it later. 
# To turn off the floppy motor is necessary write 0 on the 0x3f2 I/O port.

kill_motor:
    movw    $0x3f2, %dx
    xorb    %al, %al
    outb    %al, %dx            # out[0x3f2] = 0
    .word   0x00eb, 0x00eb      # perform short jmp delays
    ret

.org 510						# Last 2 bytes of the boot sector.

boot_flag:		.word 0xAA55	# Flag indicating a boot disk
