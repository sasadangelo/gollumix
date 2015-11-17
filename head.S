.text

.global stext
.align 4,0x90
stext:

startup_32:
	cld
	movl    $0x10,%eax		# DS = ES = FS = GS = entry 0x10 in GDT containing
	mov     %ax,%ds         # reference to data segment
	mov     %ax,%es
    mov     %ax,%fs
    mov     %ax,%gs

    call    start_kernel

done:
    jmp done
