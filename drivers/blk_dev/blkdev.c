#include <asm/system.h>
#include <asm/string.h>
#include <gollumix/stddef.h>
#include <gollumix/blkdev.h>

struct blkdev *blk_devs = NULL;

void blkdev_add(struct blkdev *bd) {
    if (!bd) {
        return;
    }

    bd->next = blk_devs;
    blk_devs = bd;
}

struct blkdev *blkdev_get(char *name) {
    struct blkdev *k;

    for(k=blk_devs; k; k=k->next) {
        if (!strcmp(name, k->name)) {
            return k;
        }
    }

    return NULL;
}

