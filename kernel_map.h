/*
 * kernel_map.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef KERNEL_MAP_H
#define KERNEL_MAP_H

#include "config.h"
#include "mm.h"

#define K_START DEF_SYSADDR                    // start address of the kernel
// The kernel start from K_START and end to K_REAL_END. The space
// between K_REAL_END and K_END is in the range [PAGE_SIZE .. 2*PAGE_SIZE-1]
// and it will contain the VFS, so there is enough room for it.
#define K_END	   (unsigned int)&__KERNEL_END__ + PAGE_SIZE
#define K_REAL_END (unsigned int)&__KERNEL_REAL_END__
#define K_SIZE     K_END - K_START             // the kernel size

extern char __KERNEL_END__, __KERNEL_REAL_END__;

#endif
