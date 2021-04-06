#include "spi.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "io.h"

void spi_init()
{
    set_MOSI_as_output();
    set_SCK_WAIT_as_output();
    set_MOSI(0);
    set_SCK_WAIT(0);

    // enable SPI, set as MASTER, clock to fosc/128
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);

    spi_deactivate();
}

void spi_activate()
{
    set_MOSI_as_output();
    set_SCK_WAIT_as_output();
    set_MOSI(0);
    set_SCK_WAIT(0);
    set_SDCS(0);
}

void spi_deactivate()
{
    set_SDCS(1);
    set_MOSI_as_high_impedance();  // release SPI lines if they need to be used to program the DMA
    set_SCK_WAIT_as_high_impedance();
}

uint8_t spi_send(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & (1 << SPIF)));
    uint8_t r = SPDR;
    return r;
}

uint8_t spi_recv()
{
    return spi_send(0xff);
}

uint8_t spi_recv_ignore_ff()
{
    uint8_t i = 0, r;
    while ((r = spi_recv()) == 0xff) {
        ++i;
        _delay_us(30);
        if (i > 8)
            break;  // timeout
    }
    return r;
}
