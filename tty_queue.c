#include "tty_queue.h"
#include "sched.h"

//-----------------------------------------------------------------------------
// PUBLIC functions
//-----------------------------------------------------------------------------

void put_queue(struct tty_queue* queue, int ch) {
    unsigned long new_head;

    // put character in the queue
    queue->buf[queue->head]=ch;
    new_head=(queue->head+1)&(TTY_BUF_SIZE-1);
    if (new_head != queue->tail) {
        queue->head=new_head;
    }

    // if there are processes waiting, woken up them
    wake_up(&queue->wait);
}

void puts_queue(struct tty_queue* queue, char *cp) {
    unsigned long new_head;
    char ch;

    while ((ch=*cp++)) {
        queue->buf[queue->head] = ch;
        new_head=(queue->head+1)&(TTY_BUF_SIZE-1);
        if (new_head != queue->tail) {
            queue->head=new_head;
        }
    }

    // if there are processes waiting, woken up them
    wake_up(&queue->wait);
}
