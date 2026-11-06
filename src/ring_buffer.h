#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>

int buffer_putc(uint8_t c);    //return 1 on success, -1 if full
int buffer_getc(void);          //return 1 on success, -1 if full

#endif