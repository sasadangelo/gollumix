/*
 * linkage.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef ASM_LINKAGE
#define ASM_LINKAGE

#define ALIGN   .align 16,0x90
#define ENTRY(name) \
	.global name; \
    ALIGN; \
    name:

#endif

