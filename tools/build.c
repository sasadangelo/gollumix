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
#include <gollumix/vfs.h>
#include <gollumix/kernel_map.h>

#define DEFAULT_MAJOR_ROOT 0
#define DEFAULT_MINOR_ROOT 0

// Number of sector for setup code (see also bootsect.S)
#define SETUP_SECTS 4
// Size of a sector
#define SECT_SIZE   0x200

// BOOT_SIZE  = size of boot sector
// SETUP_SIZE = size of setup code
// K_SIZE     = size of system image + processes (see kernel_map.h)
#define BOOT_SIZE   SECT_SIZE
#define SETUP_SIZE  SECT_SIZE*SETUP_SECTS

#define msg_print(str, args...) fprintf(stderr, str, ## args)

char buffer[BOOT_SIZE+SETUP_SIZE+K_MAX_SIZE];
char *buf_ptr = buffer;
const char *buf_end = buffer + sizeof(buffer);

#define buf_used() (int)(buf_ptr-buffer)
#define buf_free() (int)(buf_end-buf_ptr) 

const char *IMAGE = "image";

void die(const char * str, ...) {
    va_list args;
    va_start(args, str);
    vfprintf(stderr, str, args);
    fputc('\n', stderr);
    exit(1);
}

void usage(void) {
    die("Usage: build bootsect setup system N proc1 ... procN");
}

int file_read(const char *name, const int size) {
    FILE *in;
    int n;

    msg_print("Reading %s ... ", name);

    if (buf_ptr+size > buf_end) {
        die("build: not enough space to load %s", name);
    }

    in = fopen (name, "rb");

    if (!in) {
        die("build: unable to open %s", name); 
    }

    n = fread (buf_ptr, 1, buf_end-buf_ptr, in);

    buf_ptr += n;

    if(!feof(in)) {
        die("build: file %s too big", name); 
    }

    fclose (in);

    if (n != size && size != 0) {
        die("build: size error for %s", name);
    }

    msg_print("ok! (%d bytes read)\n", n);
    return n;
}

void write_image() {
    FILE *image;
    int size;

    image = fopen(IMAGE, "wb");

    if (!image) {
        die("build: unable to write kernel image");
    }

    size = fwrite(buffer, 1, buf_used(), image);

    fclose(image);

    if (buf_used() != size) {
        die("build: write error during kernel image writing");
    }
}

// This toold read bootsect, setup, system and N processes and create a kernel
// image on stdout. The caller of the tool should redirect this output on a file
int main(int argc, char ** argv) {
    // The VFS structure used for our hardcoded processes
    struct vfs_header *vfs_h;
    struct stat info;
    unsigned int num_procs;
    int ksize, size_header, proc_size, i;

    if (argc < 5) {
        usage();
    }

    num_procs = atoi(argv[4]);

    msg_print("\nRoot device is (%d, %d)\n\n", DEFAULT_MAJOR_ROOT, 
                                               DEFAULT_MINOR_ROOT);

    ///////////////////////////////////////////////////////////////////////////
    // READ BOOTSECT
    //
    // read the boot sector, making some check on it.
    // A bootsector must be 512 bytes and its final word must be 0xAA55.
    if (stat(argv[1], &info) < 0) {
        die("build: unable to stat %s", argv[1]);
    }

    if (info.st_size != SECT_SIZE) {
        die("build: invalid size for %s", argv[1]);
    }

    file_read(argv[1], info.st_size);

    if (buffer[510] != (char)0x55 || buffer[511] != (char)0xAA) {
        die("build: last word of %s must be 0xAA55", argv[1]);
    }

    buffer[508] = DEFAULT_MINOR_ROOT;
    buffer[509] = DEFAULT_MAJOR_ROOT;

    ///////////////////////////////////////////////////////////////////////////
    // READ SETUP
    //
    // read the setup code. The size of this code must be equal to 4 sectors.
    if (stat(argv[2], &info) < 0) {
        die("build: unable to stat %s", argv[2]);
    }
				    
    if (info.st_size != SETUP_SIZE) {
        die("build: invalid size for %s", argv[2]);
    }

    file_read(argv[2], SETUP_SIZE);

    ///////////////////////////////////////////////////////////////////////////
    // READ SYSTEM
    //
    // read the system code and then print it on stdout.
    ksize = file_read(argv[3], 0);

    if (((ksize + 15) / 16) > DEF_SYSSIZE) {
        die("build: the kernel is too big");
    }

    if (((ksize + 15) / 16) > 0xefff) {
        msg_print("build: kernel is too big for standalone boot from floppy\n");
    }

    ///////////////////////////////////////////////////////////////////////////
    // READ PROCESSES
    //
    // since our kernel does not support a filesystem yet, the processes code
    // are appended to the kernel image in a very simple filesystem.
    size_header = sizeof(struct vfs_header) +
                  num_procs * sizeof(struct node_struct);

    if (buf_free() < size_header) {
        die("build: VFS table too big");
    }

    vfs_h = (struct vfs_header*) buf_ptr;
    buf_ptr += size_header;
    ksize += size_header;
    vfs_h->n_files = num_procs;

    for (i = 0; i<num_procs; ++i) {
        proc_size = file_read(argv[5+i], 0);
        ksize += proc_size;
        sprintf(vfs_h->node[i].name, "PRG%d", i+1);
        vfs_h->node[i].size = proc_size;
    }

    if (ksize > K_MAX_SIZE) {
        die("build: kernel+processes exceed max size allowed");
    }

    msg_print("\nKernel (+tasks) %d bytes, %.1f Kb, %.1f%%\n\n",
              ksize, (float)ksize/1024, (float)(ksize*100/K_MAX_SIZE));

    // convert sys_size in 16 byte clicks and write in the boot sector.
    ksize = (ksize + 15) / 16;

    buffer[500] = (char)(ksize & 0xFF);
    buffer[501] = (char)((ksize >> 8) & 0xFF);

    ///////////////////////////////////////////////////////////////////////////
    // WRITE IMAGE on DISK
    //
    // write boot sector + setup + kernel + processes on disk.
    write_image(); 

    return 0;
}
