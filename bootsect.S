.code16
.text

.global _start

_start:
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
    ret

.org 510						# Last 2 bytes of the boot sector.

boot_flag:		.word 0xAA55	# Flag indicating a boot disk
