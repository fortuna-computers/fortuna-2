#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdint.h>

void    serial_init();
uint8_t serial_recv();

#define RED   "\e[0;31m"
#define GRN   "\e[0;32m"
#define RESET "\e[0m"

#endif
