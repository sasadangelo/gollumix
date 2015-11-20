/*
 * usercopy.c
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */
#include <asm/uaccess.h>

// TODO: improve perfromance
//
// This routine copy a null terminated string from user space to kernel
// space. The string is truncated if it is greater than count characters.
long strncpy_from_user(char *dst, const char *src, long count) {
	long i;

	for(i=0; i<count; i++) {
		*dst = (char)get_user_byte(src);
		if(*dst == '\0')
			break;
		src++;
		dst++;
	}
	return i;
}

// TODO: improve perfromance
//
// This routine copy n byte from kernel space to user space.
long copy_to_user(char *dst, const char *src, long count) {
    long i;

    for(i=0; i<count; i++) {
        put_user_byte(*src, dst);
        src++;
        dst++;
    }
    return i;
}

