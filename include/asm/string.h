#ifndef STRING_H
#define STRING_H

#include <gollumix/stddef.h>

/*
 * This string-include defines all string functions as inline
 * functions. Use gcc. It also assumes ds=es=data space, this should be
 * normal. Most of the string-functions are rather heavily hand-optimized,
 * see especially strtok,strstr,str[c]spn. They should work, but are not
 * very easy to understand. Everything is done entirely within the register
 * set, making the functions fast and clean. String instructions have been
 * used through-out, making for "slightly" unclear code :-)
 *
 *		NO Copyright (C) 1991, 1992 Linus Torvalds,
 *		consider these trivial functions to be PD.
 */

static inline void * memmove(void * dest,const void * src, size_t n)
{
int d0, d1, d2;
if (dest<src)
__asm__ __volatile__(
	"rep\n\t"
	"movsb"
	: "=&c" (d0), "=&S" (d1), "=&D" (d2)
	:"0" (n),"1" (src),"2" (dest)
	: "memory");
else
__asm__ __volatile__(
	"std\n\t"
	"rep\n\t"
	"movsb\n\t"
	"cld"
	: "=&c" (d0), "=&S" (d1), "=&D" (d2)
	:"0" (n),
	 "1" (n-1+(const char *)src),
	 "2" (n-1+(char *)dest)
	:"memory");
    return dest;
}

static inline size_t strlen(const char * s) {
	int d0;
	register int __res;
	__asm__ __volatile__(
	    "repne\n\t"
	    "scasb\n\t"
	    "notl %0\n\t"
	    "decl %0"
	    :"=c" (__res), "=&D" (d0) :"1" (s),"a" (0), "0" (0xffffffff));
	return __res;
}

static inline void *memcpy(void * to, const void * from, size_t n)
{
    int d0, d1, d2;
    __asm__ __volatile__(
        "rep ; movsl\n\t"
        "testb $2,%b4\n\t"
        "je 1f\n\t"
        "movsw\n"
        "1:\ttestb $1,%b4\n\t"
        "je 2f\n\t"
        "movsb\n"
        "2:"
        : "=&c" (d0), "=&D" (d1), "=&S" (d2)
        :"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
        : "memory");
    return (to);
}

static inline void *memset(void * s, char c, size_t count)
{
    int d0, d1;
	__asm__ __volatile__(
        "rep\n\t"
        "stosb"
        : "=&c" (d0), "=&D" (d1)
        :"a" (c),"1" (s),"0" (count)
        :"memory");
    return s;
}

static inline int strcmp(const char * cs,const char * ct) {
    int d0, d1;
    register int __res;
__asm__ __volatile__(
    "1:\tlodsb\n\t"
    "scasb\n\t"
    "jne 2f\n\t"
    "testb %%al,%%al\n\t"
    "jne 1b\n\t"
    "xorl %%eax,%%eax\n\t"
    "jmp 3f\n"
    "2:\tsbbl %%eax,%%eax\n\t"
    "orb $1,%%al\n"
    "3:"
    :"=a" (__res), "=&S" (d0), "=&D" (d1)
    :"1" (cs),"2" (ct));
    return __res;
}
/*
static inline void memcpy_tofs(void * to, void * from, unsigned long n)
{
__asm__("cld\n\t"
    "push %%es\n\t"
    "push %%fs\n\t"
    "pop %%es\n\t"
    "testb $1,%%cl\n\t"
    "je 1f\n\t"
    "movsb\n"
    "1:\ttestb $2,%%cl\n\t"
    "je 2f\n\t"
    "movsw\n"
    "2:\tshrl $2,%%ecx\n\t"
    "rep ; movsl\n\t"
    "pop %%es"
    ::"c" (n),"D" ((long) to),"S" ((long) from)
    :"cx","di","si");
}
*/
#endif
