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

# Enable A20
    call    empty_8042
    movb    $0xd1, %al
    outb    %al, $0x64      # out[0x64] = 0xD1 -> command: write to output 
                            # buffer
    call    empty_8042		
    movb    $0xdf, %al
    outb    %al, $0x60      # out[0x60] = 0xDF -> bit 1 = 1 -> enable A20
    call    empty_8042

# Reset coprocessor
    xorw    %ax, %ax
    outb    %al, $0xf0      # out[0xF0] = 0
    call    delay
    outb    %al, $0xf1      # out[0xF1] = 0
    call    delay

# PIC programming
    movb    $0x11, %al      # Comando ICW1 (out[0x20] = 0x11, Master). ICW4
    outb    %al, $0x20      # will be sent and size of entry in interrupt
    call    delay           # vector is 4 bytes.

    outb    %al, $0xa0      # Comando ICW1. (out[0xA0] = 0x11, Slave). ICW4
    call    delay           # will be sent and size of entry in interrupt
                            # vector is 4 bytes.

    movb    $0x20, %al      # Comando ICW2. (out[0x21] = 0x20, Master).
    outb    %al, $0x21      # IRQ0 -> 20h, IRQ1 -> 21h, and so on
    call    delay

    movb    $0x28, %al      # Comando ICW2. (out[0xA1] = 0x28, Slave).
    outb    %al, $0xa1      # IRQ8 -> 28h, IRQ9 -> 29h, and so on
    call    delay

    movb    $0x04, %al      # Command ICW3. The controller master is attached to
    outb    %al, $0x21      # the slave through IRQ12 (IRQ4 on slave).
    call    delay


    movb    $0x02, %al      # Command ICW3. The controller slave is attached to
    outb    %al, $0xa1      # the master through IRQ2
    call    delay

    movb    $0x01, %al      # Command ICW4. (out[0x21] = 0x01, Master).
    outb    %al, $0x21
    call    delay

    outb    %al, $0xa1      # Command ICW4. (out[0xA1] = 0x01, Slave).
    call    delay

    movb    $0xff, %al      # Command OCW1. (out[0xA1] = 0xFF, Master).
    outb    %al, $0xa1      # mask all interrupt for controller Slave
    call    delay

    movb    $0xfb, %al      # Command OCW1. (out[0x21] = 0xFB, Slave).
    outb    %al, $0x21      # Mask all interrupt except IRQ2

# Switch in Protected mode setting the bit 0 of the Machine Status Word (MSW)
    movw    $0x0001, %ax
    lmsw    %ax
    call    delay

    .byte   0x66, 0xea      # jmpi to the logic address (8, 0x1000).
code32: .long   0x1000      # 8 is the selector and point to the second entry
    .word   8               # in GDT (code segment) whose start address is 0.
                            # So 0+0x1000=0x1000 -> jmpi 0x1000.

done:
	jmp done

# empty_8042
# This routine empty the keyboard command queue.
# The keyboard Status Register at port 0x64 has two bits (0 and 1) that
# specify if there are data in the keyboard queue. Bit 0 says if keyboard
# data are in queue (if equal to 1), Bit 0 says if user data are in queue
# (if equal to 1). These two bits must be 0.

empty_8042:
    call    delay           # a small delay
    inb $0x64, %al          # %al = in[0x64]
    testb   $0x1, %al
    jz  no_output           # if (bit %al[0] == 0) jump no_output
    call    delay           # a small delay
    inb $0x60, %al          # %al = in[0x60]
    jmp empty_8042          # jump to empty_8042
no_output:
    testb   $2, %al         # if (bit %al[1] == 1) jump empty_8042
    jnz empty_8042
    ret

delay:
    .word   0x00eb      # make a short jump call jmp $+2
    ret

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

