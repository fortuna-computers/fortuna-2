#include "io.h"

volatile uint8_t last_char_received = 0;

void io_init()
{
}

void io_set_last_char_received(uint8_t chr)
{
    last_char_received = chr;
}

uint8_t io_last_char_received()
{
    uint8_t tmp = last_char_received;
    last_char_received = 0;
    return tmp;
}
