/*
 *  $Id: build.c,v 1.5 1997/05/19 12:29:58 mj Exp $
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 1997 Martin Mares
 */

/*
 * This file builds a disk-image from three different files:
 *
 * - bootsect: exactly 512 bytes of 8086 machine code, loads the rest
 * - setup: 8086 machine code, sets up system parm
 * - system: 80386 code for actual system
 *
 * It does some checking that all files are of the correct type, and
 * just writes the result to stdout, removing headers and padding to
 * the right amount. It also writes some system data to stderr.
 */

#include <stdio.h>
#include <asm/boot.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../vfs.h"

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long u32;

#define DEFAULT_MAJOR_ROOT 0
#define DEFAULT_MINOR_ROOT 0

// Minimal number of setup sectors (see also bootsect.S)
#define SETUP_SECTS 4

byte buf[1024];
int fd;

void die(const char * str, ...)
{
    va_list args;
    va_start(args, str);
    vfprintf(stderr, str, args);
    fputc('\n', stderr);
    exit(1);
}

void file_open(const char *name)
{
    if ((fd = open(name, O_RDONLY, 0)) < 0)
        die("Unable to open `%s': %m", name);
}

void usage(void)
{
    die("Usage: build bootsect setup system N proc1 ... procN [> image]");
}

int main(int argc, char ** argv)
{
    unsigned int i, c, sz, setup_sectors;
    unsigned int num_procs, size_header;
    byte major_root, minor_root;
    u32 sys_size, size_procs;
    struct stat sb;
    byte *buffer;
    int n;

    // The VFS structure used for our hardcoded processes
    struct vfs_header *vfs_h;

    if (argc < 5)
        usage();

    num_procs = atoi(argv[4]);

    major_root = DEFAULT_MAJOR_ROOT;
    minor_root = DEFAULT_MINOR_ROOT;
	
    fprintf(stderr, "Root device is (%d, %d)\n", major_root, minor_root);

    file_open(argv[1]);
    i = read(fd, buf, sizeof(buf));
    fprintf(stderr,"Boot sector %d bytes.\n",i);
    if (i != 512)
        die("Boot block must be exactly 512 bytes");
    if (buf[510] != 0x55 || buf[511] != 0xaa)
        die("Boot block hasn't got boot flag (0xAA55)");
    buf[508] = minor_root;
    buf[509] = major_root;
    if (write(1, buf, 512) != 512)
        die("Write call failed");
    close (fd);

    file_open(argv[2]);  // Copy the setup code
    for (i=0 ; (c=read(fd, buf, sizeof(buf)))>0 ; i+=c )
        if (write(1, buf, c) != c)
            die("Write call failed");
    if (c != 0)
        die("read-error on `setup'");
    close (fd);

    setup_sectors = (i + 511) / 512; // Pad unused space with zeros
    // for compatibility with ancient versions of LILO.
    if (setup_sectors < SETUP_SECTS)
        setup_sectors = SETUP_SECTS;
    fprintf(stderr, "Setup is %d bytes.\n", i);
    memset(buf, 0, sizeof(buf));
    while (i < setup_sectors * 512) {
        c = setup_sectors * 512 - i;
        if (c > sizeof(buf))
            c = sizeof(buf);
        if (write(1, buf, c) != c)
            die("Write call failed");
        i += c;
    }

    file_open(argv[3]);
    if (fstat (fd, &sb))
        die("Unable to stat `%s': %m", argv[3]);
    sz = sb.st_size;
    fprintf (stderr, "System is %d kB\n", sz/1024);
    sys_size = sz;
	// 0x28000*16 = 2.5 MB, conservative estimate for the current maximum
    if (((sys_size + 15) / 16) > DEF_SYSSIZE)
        die("System is too big"); 
    if (((sys_size + 15) / 16) > 0xefff)
        fprintf(stderr,"warning: kernel is too big for standalone boot "
                       "from floppy\n");
    while (sz > 0) {
        int l, n;

        l = (sz > sizeof(buf)) ? sizeof(buf) : sz;
        if ((n=read(fd, buf, l)) != l) {
            if (n < 0)
                die("Error reading %s: %m", argv[3]);
            else
                die("%s: Unexpected EOF", argv[3]);
        }
        if (write(1, buf, l) != l)
            die("Write failed");
        sz -= l;
    }
    close(fd);

    // ----------------------------------------------------------------------
    // Here I assume the procs are very small, so they could be included in
    // the buffer.
    size_header = sizeof(struct vfs_header) + 
                  num_procs * sizeof(struct node_struct);

    sys_size += size_header;

    buffer = buf;
    vfs_h = (struct vfs_header*) buf;
    vfs_h->n_files = num_procs;
    buffer += size_header;
    size_procs = size_header;

    for (i = 0; i<num_procs; ++i) {
        file_open(argv[5+i]);
        if (fstat (fd, &sb)) {
            die("Unable to stat `%s': %m", argv[5+i]);
        }
        sz = sb.st_size;
        size_procs += sz;
        sys_size += sz;
        vfs_h->node[i].size = sz;
        sprintf(vfs_h->node[i].name, "PRG%d", i+1);

        if ((n=read(fd, buffer, sz)) != sz) {
            if (n < 0)
                die("Error reading %s: %m", argv[5+i]);
            else
                die("%s: Unexpected EOF", argv[5+i]);
        }

        buffer += sz;

        close(fd);

        fprintf(stderr, "Added process: %s, size: %d\n", 
                        vfs_h->node[i].name,
                        vfs_h->node[i].size);
    }

    // write buf on stdout
    write(1, buf, size_procs);

    // convert sys_size in 16 byte clicks.
    sys_size = (sys_size + 15) / 16;

    if (lseek(1, 507, SEEK_SET) != 507) // Write sizes to the bootsector
        die("Output: seek failed");
    buf[0] = setup_sectors;
    if (write(1, buf, 1) != 1)
        die("Write of setup sector count failed");
    if (lseek(1, 508, SEEK_SET) != 508)
        die("Output: seek failed");
    buf[0] = (sys_size & 0xff);
    buf[1] = ((sys_size >> 8) & 0xff);
    if (write(1, buf, 2) != 2)
        die("Write of image length failed");

    return 0; // Everything is OK
}
