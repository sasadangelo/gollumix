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

// Start address of the kernel
#define K_START    DEF_SYSADDR
// Kernel (+ tasks) max size
#define K_MAX_SIZE (30*PAGE_SIZE)
// Kernel (+tasks) end address
#define K_END      (K_START+K_MAX_SIZE)

#endif
