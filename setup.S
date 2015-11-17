SETUPSEG = 0x9020

.code16
.text

.global _start
_start:
    .word   0x00eb, 0x00eb      # short jump delay
    movw    $0x9000, %ax
    movw    %ax, %ds

# Ok, disable interrupt and NMI bootup.
    cli
    movb    $0x80, %al          # disable NMI bootup
    outb    %al, $0x70          # out[0x80] = 0x70

# Move the system from 0x1000:0 to 0x0100:0. The copy is by 4Kb blocks

    movw    $0x100, %ax
    movw    $0x1000, %bx
    cld

do_move:
    movw    %ax, %es            # ES:DI = destination address
    addw    $0x100, %ax
    cmpw    $0x9000, %ax        # if (%ax == 0x9000) jump to end_move
    jz  end_move
    movw    %bx, %ds            # DS:SI = source address
    addw    $0x100, %bx
    subw    %di, %di
    subw    %si, %si
    movw    $0x800, %cx         # %cx = 0x800 (2048 words == 4096 bytes == 4Kb)
    rep
    movsw

    jmp do_move

end_move:
    movw    $SETUPSEG, %ax
    movw    %ax, %ds            # %ds = $SETUPSEG

# This is the memory situation after the copy.
#
#    ---------------
#   |               | 0x0000:0
#   |     ....      |
#   |_______________|
#   |     COPY      | 0x0100:0
#   |     IMAGE     |
#   |     KERNEL    |
#   |    (main.c)   |
#   |               |
#   |_______________|
#   |               |
#   |     ....      |
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
#   |     KERNEL    | (size 0x7F00)
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

# Setting up the system.

    leaw    msg, %si        # Print the message "Setting up system"
    call    prtstr

# Set the IDTR and GDTR registers
    lidt    idt_48          # base == 0, limit == 0
    lgdt    gdt_48          # limit == 2048 -> 256 entries

done:
	jmp done

prtstr: lodsb               # load the character to print from DS:SI to AX
        andb    %al,%al
        jz      fin         # print the character until AL is 0x0
        call    prnt1       # print the character at DS:SI
        jmp     prtstr
fin:    ret

prnt1:                      # use the service 0x0E of interrupt 0x10 to print
                            # the character in %al.
        pushw   %ax
        pushw   %cx
        xorb    %bh,%bh
        movw    $0x01, %cx
        movb    $0x0e, %ah
        int     $0x10
        popw    %cx
        popw    %ax
        ret

# First three entries of GDT

gdt:

# Dummy Descriptor

        .word   0, 0, 0, 0

# Kernel Code Segment Descriptor
                        
        .word   0xFFFF      # 4Gb - (0x100000*0x1000 = 4Gb)
        .word   0           # base address = 0
        .word   0x9A00      # code read/exec
        .word   0x00CF      # granularity = 4096, 386
                            #  (+5th nibble of limit)

# Kernel Data Segment Descriptor

        .word   0xFFFF      # 4Gb - (0x100000*0x1000 = 4Gb)
        .word   0           # base address = 0
        .word   0x9200      # data read/write
        .word   0x00CF      # granularity = 4096, 386
                            #  (+5th nibble of limit)

# Values for IDTR and GDTR
idt_48:
        .word   0           # idt limit = 0
        .word   0, 0        # idt base = 0L
gdt_48:
        .word   0x8000      # gdt limit=2048, 256 GDT entries
        .long   gdt + SETUPSEG*0x10

msg:    .byte 13, 10
        .ascii "Setting up system"
        .byte 0x0

.org 2048

