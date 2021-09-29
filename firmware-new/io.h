#ifndef IO_H_
#define IO_H_

#include <stdint.h>

void    io_set_last_char_received(uint8_t chr);
uint8_t io_last_char_received();

#endif