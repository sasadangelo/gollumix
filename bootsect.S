SETUPSECTS  = 4
BOOTSEG = 0x07C0
INITSEG = 0x9000
SETUPSEG= 0x9020

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

    pushw   %ax             # dump the error code on the video
    call    print_nl        # print a new line on the video
    movw    %sp, %bp        # ??
    call    print_hex       # print %ax on the video
    popw    %ax
    jmp     load_setup      # retry again

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

    movw    $INITSEG, %ax   # %ax = 0x9000
    movw    %ax, %es        # %es =  %ax

# print the "Loading system " message.

    movb    $0x03, %ah      # read the cursor current position using
                            # service 0x03 of interrupt 0x10.
    xorb    %bh, %bh
    int $0x10               # get the cursor position. %dh == row,
                            # %dl == col

    movw    $17, %cx        # %cx = 17 (characters in the string)
    movw    $0x0007, %bx    # %bh (page) = 0, %bl (attribute) = 7 (normal)
    movw    $msg1, %bp      # %es:%bp point to the string to write
    movw    $0x1301, %ax    # write string at row %dh and col %dl
                            # and move cursor
    int     $0x10
	call kill_motor         # stop the floppy motor

# After that (everything loaded), we jump to the setup-routine
# loaded directly after the bootblock.

    ljmp    $SETUPSEG, $0

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

# print_all
# This routine is for debugging purposes.
#
# It will print out the registers AX, BX, CX e DX.  The assumption is that 
# it is called with a stack frame like this
#
#   (error) <- %sp
#   %ax
#   %bx
#   %cx
#   %dx

print_all:
    movw    $5, %cx     # %cx = num of parameters
    movw    %sp, %bp

print_loop:
    pushw   %cx
    call    print_nl    # print new line
    cmpb    $5, %cl     # if cl is 0 then the reg name is not necessary.
                        # The first loop print the error code, a jump
                        # to no_reg is made.
    jae no_reg

    movw    $0xe05 + 'A' - 1, %ax   # %ah = 0x0E %al = 'D'
    subb    %cl, %al    # depending on the value of %cl, %al may
                        # contain the following characters: 'A', 'B'
                        # 'C' e 'D'.
    int $0x10           # print the character
    movb    $'X', %al   # print 'X'
    int $0x10
    movb    $':', %al   # print ':'
    int $0x10

no_reg:                 # print the value on top of the stack
    addw    $2, %bp     # next register
    call    print_hex   # print the value in hex
    popw    %cx         # %cx = top of the stack
    loop    print_loop  # loop until %cx is 0
    ret

# print_hex
# This routine is for debugging purposes, and prints the word
# pointed to by ss:bp in hexadecimal.

print_hex:
    movw    $4, %cx     # 4 hex digits
    movw    (%bp), %dx  # load word to print in %dx

print_digit:
    rolw    $4, %dx     # rotate to use low 4 bits
    movw    $0xe0f, %ax # ah = 0x0E %al = 0x0F
    andb    %dl, %al    # %al = %al & %dl -> %al contain the value to
                        # print
    addb    $0x90, %al  # convert %al in ASCII and print it
    daa
    adc     $0x40, %al
    daa
    int $0x10
    loop    print_digit # loop until %cx is 0
    ret

# print_nl
# This routine print a simple new line
print_nl:
    movw    $0xe0d, %ax     # print CR
    int     $0x10
    movb    $0xa, %al       # print LF
    int     $0x10
    ret

# This boot message start with a new line (CR=13, LF=10). So the string length
# is 17 characters.
msg1:       .byte 13, 10
            .ascii "Loading system "

.org 509						# Last 3 bytes of the boot sector.

setup_sects:    .byte SETUPSECTS
boot_flag:		.word 0xAA55	# Flag indicating a boot disk
