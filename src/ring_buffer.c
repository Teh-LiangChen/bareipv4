#include "ring_buffer.h"

#define BUFFER_SIZE 8

static uint8_t ring_buffer[BUFFER_SIZE];

static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;

int buffer_putc(uint8_t c)
{
    uint8_t next;
    next = (head + 1) & (BUFFER_SIZE - 1);
    if (next == tail) return -1;

    ring_buffer[head] = c;
    head = next;
    return 1;
}

int buffer_getc(void)
{
    if (tail == head) return -1;
    uint8_t c = ring_buffer[tail];
    tail = (tail + 1) & (BUFFER_SIZE - 1);
    return c;
}