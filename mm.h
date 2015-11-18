/*
 * mm.h
 *
 * Copyright (C) 2003 Open Community
 * author Salvatore D'Angelo (koala.gnu@tiscali.it)
 */

#ifndef MM_H
#define MM_H

#define PAGE_SIZE   4096
#define USER_STACK_SIZE 1024

/*
 * This is the interface of the memory management unit.
 * They are implemented in  mm.c 
 */
void* get_free_page(void);
void  free_page(void* page);
void* get_free_pages(unsigned int num);
void  free_pages(void* page, unsigned int nr);

#endif
