/*
 * kernel_map.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef KERNEL_MAP_H
#define KERNEL_MAP_H

#include "config.h"

#define K_START DEF_SYSADDR                    // start address of the kernel
#define K_END   (unsigned int)&__KERNEL_END__  // end address of the kernel
#define K_SIZE  K_END - K_START                // the kernel size

extern char __KERNEL_END__;

#endif
