/*
 * bootsect.S
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <gollumix/config.h>

SETUPSECTS = 4
BOOTSEG    = 0x07C0
INITSEG    = DEF_INITSEG
SETUPSEG   = DEF_SETUPSEG
SYSSEG     = DEF_SYSSEG
SYSSIZE    = DEF_SYSSIZE        # This value is put in the syssize variable
                                # (see below). The value reported here is a
                                # dummy value, it will be replaced by the
                                # build.c utility. Please see the code in
                                # tools/build.c.
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
#   |               |
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

    xorw    %dx, %dx            # Copy the SETUPSECTS sectors from disk using
    movb    $0x02, %cl          # the service 0x02 of interrupt 0x13. The disk 
    movw    $0x0200, %bx        # start address is (0, 0, 0, 2) and the sectors 
    movb    $0x02, %ah          # will be copied in memory at the address 
    movb    setup_sects, %al    # 0x9000:0x0200
    int     $0x13

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
#   |   BOOTSECTOR  | 0x07C0:0
#   |               |
#   |go: ....       |
#   |               |
#   |_______________|
#   |               |
#   |   .........   |
#   |_______________|
#   |     COPY      | 0x9000:0
#   |   BOOTSECTOR  | 
#   |               |
#   |go: ....       |
#   |               |
#   |_______________|
#   |               | 0x9000:0x0200
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
# the address 0x4000 is an arbitrary value >= bootsect size +
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

                            # start loading the kernel at 0x1000:0
    movw    $SYSSEG, %ax
    movw    %ax, %es
    call read_it
	call kill_motor         # stop the floppy motor

# Basically in the memory there is the following situation:
#
#    ---------------
#   |               |
#   |_______________|
#   |   BOOTSECTOR  | 0x07C0:0
#   |               |
#   |go: ....       |
#   |               |
#   |_______________|
#   |               |
#   |   .........   |
#   |_______________|
#   |               | 0x1000:0
#   |     IMAGE     |
#   |     KERNEL    |
#   |    (main.c)   |
#   |_______________|
#   |   .........   |
#   |_______________|
#   |     COPY      |
#   |   BOOTSECTOR  | 0x9000:0
#   |               |
#   |go: ....       |
#   |               |
#   |_______________|
#   |    (head.S)   | 0x9000:0x200 = 0x90200
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
# the address 0x4000 is an arbitrary value >= bootsect size +
#                               setup size    +
#                               stack size

# After that (everything loaded), we jump to the setup-routine
# loaded directly after the bootblock.

    ljmp    $SETUPSEG, $0

# drive is assumed to be 0
sread:      .word 0             # the current sector to read
head:       .word 0             # the current head to read
track:      .word 0             # the current track to read

# read_it
#
# This routine is used to transfer the kernel stored at disk address
# (drive=0, head=0, track=0, sector=5) in the RAM memory starting
# at the address 0x1000:0. The interrupt 0x13 is used to execute this
# operation.
# A segment of 64Kb (64512) may contain 7 track of 18 sector each. Here we
# read a track at a time (if possible). %es contain the reference to the 
# destination segment and %bx the destination offset. When %bx cross the
# 64Kb boundaries %es must be incremented and %bx updated accordingly

read_it:
    movb    setup_sects, %al
    incb    %al
    movb    %al, sread          # sread = 5 -> this is because the read start
                                # from sector 5.
    movw    %es, %ax            # AX = ES (0x1000)
    testw   $0x0fff, %ax        # this instruction test that ES has one
                                # of the following values: 0x1000, 0x2000, ...,
                                # 0x9000. Basically we check that an invalid
                                # value has not been used for $SYSSEG.
die:
    jne die

    xorw    %bx, %bx            # ES:BX will contain the destination address
                                # of the copy 0x1000:0

rp_read:
                                # Here start the loop.

                                # Here a check is made to see if all the
                                # necessary bytes have been read.
    movw    %es, %ax
    subw    $SYSSEG, %ax
    movw    %bx, %cx
    shr $4, %cx
    add %cx, %ax                # AX = kernel bytes read
    cmpw    syssize, %ax        # if (all bytes have been read)
                                #       jump ok1_read -> need to read
                                # otherwise
                                #       stop reading. You have finished.
    jbe ok1_read
    ret

ok1_read:
                                # ES contain the current segment and BX
                                # the offset in that segment where finished the
                                # last read_track.

    movw    sectors, %ax        # AX = sector to read in the track (first loop
                                # is 18)
    subw    sread, %ax          # AX = AX - sread (subctract from AX the
                                # sectors already read -> in the first loop
                                # sread == 5 and AX == 13)
    movw    %ax, %cx
    shlw    $9, %cx
    addw    %bx, %cx            # CX = the offset where the reading of the
                                # track will finish. if CX does not cross
                                # the 64Kb boundaries go to ok2_read,
                                # otherwise a less number of sectors must be
                                # read.
    jnc ok2_read
    je  ok2_read

                                # the next read track cross the 64 Kb
                                # boundaries so less sectors must be read. But
                                # how many sector I can read? It is easy,
                                # calculate the distance between BX and the
                                # end of the segment and divide this value for
                                # 512.
    xorw    %ax, %ax
    subw    %bx, %ax            # AX == distance between BX and the end of the
                                # segment
    shrw    $9, %ax             # AX == sectors to read

ok2_read:
    call    read_track          # ok, read the track ....

    movw    %ax, %cx            # %ax contain the sectors read, sread contain
                                # the start sector
    addw    sread, %ax          #
    cmpw    sectors, %ax        # if we do not have read the track completely
                                # jump to ok3_read
    jne ok3_read
                                # If I well understand the reading of the
                                # track will be in the following order:
                                #
                                #           (drive, head, track)
                                #           (0, 0, 0)
                                #           (0, 1, 0)
                                #           (0, 0, 1)
                                #           (0, 1, 1)
                                #              ....
                                # this to reduce the head movement.

                                # a track has been read completely so here
                                # there are two cases:
                                #
                                # a) for head n has been read both the
                                #    tracks. In this case we have
                                #
                                #           head = 1-n
                                #           track = track+1
                                #
                                # b) for head n only a track has been
                                # read. In this case we have:
                                #
                                #           head = 1-n
    movw    $1, %ax             #
    subw    head, %ax           #
    jne ok4_read                # jump to ok4_read if only 1 track has been read
                                # for the head

    incw    track               # ok, both the tracks have been read for the
                                # head. Increment track.
ok4_read:
    movw    %ax, head           # head = AX
    xorw    %ax, %ax            # AX = 0 -> in this case the reading start
                                # from sector 0
ok3_read:
    movw    %ax, sread          # AX contain the sector where the read start
                                # in the current track. Move this value in sread
    shlw    $9, %cx             # CX = CX *512 -> byte read
    addw    %cx, %bx            # BX = BX + CX -> move BX to the offset where
                                # will start the next read_track.
    jnc rp_read                 # if no 64Kb boundaries has been crossed goto
                                # rp_read to continue the reading process

    movw    %es, %ax            # incrementing BX we crossed the 64Kb
                                # boundaries, so ES must point to the next
                                # segment and BX must be 0
    addb    $0x10, %ah
    movw    %ax, %es
    xorw    %bx, %bx
    jmp rp_read

read_track:
    pusha                       # save registers (ax, cx, dx, bx, sp, bp, si
                                # di)
    pusha                       # save registers

    movw    $0xe2e, %ax         # write a "." for each track read
    movw    $7, %bx
    int $0x10

    popa                        # reload registers

                                # use the service 2 of interrupt 0x13 to
                                # read the (drive, head, track, sector).
                                # See above for specification abount this
                                # service.
                                # The data are set in the following way:
                                # %dh = 0 (drive 0)
                                # %dl = head
                                # %ch = track
                                # %cl = sread
                                # %es:%bx destination address
                                # %ah = 0x02 (service)
                                # %al = the sectors to read (first loop == 13)
    movw    track, %dx
    movw    sread, %cx
    incw    %cx
    movb    %dl, %ch
    movw    head, %dx
    movb    %dl, %dh
    andw    $0x0100, %dx
    movb    $2, %ah
    pushw   %dx                 # save %ax, %bx, %cx, %dx for dump
    pushw   %cx
    pushw   %bx
    pushw   %ax
    int $0x13
    jc  bad_rt

    addw    $8, %sp             # 4 registers have been put on the stack.
                                # Discard them.
    popa                        # Recover the registers.
    ret

bad_rt:
    pushw   %ax                 # save error code
    call    print_all           # dump registers
    xorb    %ah, %ah            # %ah = 0
    xorb    %dl, %dl            # %dl = 0
    int $0x13                   # reset FDC
    addw    $10, %sp            # 4 registers + 1 error code have been put
                                # on the stack. Discard them.
    popa                        # Recover the registers.
    jmp read_track

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

# Change these setting a compile time
# Disk 1.44Mb disks:
sectors:    .word 18        # sectors = 18
# Disk 1.2Mb disks:
# sectors:  .word 15        # sectors = 15
# Disk 720Kb disks:
# sectors:  .word 9         # sectors = 9

# This boot message start with a new line (CR=13, LF=10). So the string length
# is 17 characters.
msg1:       .byte 13, 10
            .ascii "Loading system "

.org 497

# The value in setup_sects and syssize will be replace in the build.c utility.
# Please see the tools/build.c.
setup_sects: .byte SETUPSECTS
root_flags:  .word 1        # root read-only
syssize:     .word SYSSIZE
swap_dev:    .word 0
ram_size:    .word 0
vid_mode:    .word 0
root_dev:    .word 0
boot_flag:   .word 0xAA55   # Flag indicating a boot disk
