/*
 * mm.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef MM_H
#define MM_H

#define PAGE_SIZE   4096

/*
 * This is the interface of the memory management unit.
 * They are implemented in  mm.c 
 */
void* get_free_page(void);
void  free_page(void* page);

#endif
