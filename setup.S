.code16
.text

.global _start
_start:
    movb    $0xe, %ah           # Function 0x0E of Interrupt 0x10
	movb    $13, %al            # print new line
	int     $0x10
    movb    $10, %al
    int     $0x10
    movb    $'W', %al           # write 'W'
    int     $0x10
    movb    $'o', %al           # write 'o'
    int     $0x10
    movb    $'w', %al           # write 'w'
    int     $0x10
    movb    $' ', %al           # write ' '
    int     $0x10
    movb    $'I', %al           # write 'I'
    int     $0x10
    movb    $' ', %al           # write ' '
    int     $0x10
    movb    $'a', %al           # write 'a'
    int     $0x10
    movb    $'m', %al           # write 'm'
    int     $0x10
    movb    $' ', %al           # write ' '
    int     $0x10
    movb    $'i', %al           # write 'i'
    int     $0x10
    movb    $'n', %al           # write 'n'
    int     $0x10
    movb    $' ', %al           # write ' '
    int     $0x10
    movb    $'s', %al           # write 's'
    int     $0x10
    movb    $'e', %al           # write 'e'
    int     $0x10
    movb    $'t', %al           # write 't'
    int     $0x10
    movb    $'u', %al           # write 'u'
    int     $0x10
    movb    $'p', %al           # write 'p'
    int     $0x10

done:
	jmp done

.org 2048

