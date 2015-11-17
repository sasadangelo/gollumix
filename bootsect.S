SETUPSECTS  = 4
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

# This piece of code load the setup code from disk in memory. If the operation
# is successful the "Setup Loaded!!" message is displayed, otherwise an error
# message will apear.

load_setup:
    xorb    %ah, %ah            # Reset the Floppy Disk Controller (FDC) using
    xorb    %dl, %dl            # the service 0x00 of interrupt 0x13
    int     $0x13

    xorw    %dx, %dx
    movb    $0x02, %cl          # Copy the SETUPSECTS sectors from disk using
    movw    $0x0200, %bx        # the service 0x02 of interrupt 0x13. The disk
    movb    $0x02, %ah          # start address is (0, 0, 0, 2) and the sectors
    movb    setup_sects, %al    # will be copied in memory at the address
    int     $0x13               # 0x9000:0x0200

    jnc ok_load_setup

    movb $0xE, %ah              # print error message 
    movb $'E', %al
    int  $0x10
    movb $0xE, %ah
    movb $'r', %al
    int  $0x10
    movb $0xE, %ah
    movb $'r', %al
    int  $0x10
    movb $0xE, %ah
    movb $'o', %al
    int  $0x10
    movb $0xE, %ah
    movb $'r', %al
    int  $0x10
    jmp load_setup              # retry again
 
ok_load_setup:

# In memory we have the following situation.
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
#   |     COPY      |
#   |   BOOTSECTOR  | 0x9000:0 = 0x90000
#   |               |
#   |go: ....       |
#   |               |
#   |_______________|
#   |               | 0x9000:0x200 = 0x90200
#   |               |
#   |     SETUP     | 4 sectors = 2048 bytes
#   |               |
#   |_______________|
#   |               |
#   |               |
#   |     STACK     |
#   |               |
#   |_______________| <- 0x9000:0x4000-12
#   |   DISK PARMS  |
#   |   (12 bytes)  |
#   |_______________| <- 0x9000:0x4000
#
# 0x4000 is an arbitrary value >= bootsect size +
#                                 setup size    +
#                                 stack size

# print the "Setup Loaded!!!" message.

    movb $0xE, %ah 
    movb $'S', %al
    int  $0x10
    movb $'e', %al
    int  $0x10
    movb $'t', %al
    int  $0x10
    movb $'u', %al
    int  $0x10
    movb $'p', %al
    int  $0x10
    movb $' ', %al
    int  $0x10
    movb $'L', %al
    int  $0x10
    movb $'o', %al
    int  $0x10
    movb $'a', %al
    int  $0x10
    movb $'d', %al
    int  $0x10
    movb $'e', %al
    int  $0x10
    movb $'d', %al
    int  $0x10
    movb $'!', %al
    int  $0x10
    movb $'!', %al
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

.org 509						# Last 3 bytes of the boot sector.

setup_sects:    .byte SETUPSECTS
boot_flag:		.word 0xAA55	# Flag indicating a boot disk
