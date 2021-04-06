#ifndef SPI_HH_
#define SPI_HH_

#include <stddef.h>
#include <stdint.h>

#include "serial.h"

void spi_init();

void spi_activate();
void spi_deactivate();

uint8_t spi_send(uint8_t byte);
uint8_t spi_recv();
uint8_t spi_recv_ignore_ff();

#endif
