#include "spi.h"

#include <avr/io.h>
#include <util/delay.h>

void spi_init()
{
    DDRB |= (1 << PB4) | (1 << PB5) | (1 << PB7);   // MOSI, SCK, SD_CS: output
    DDRB &= ~(1 << PB6);  // MISO: input
    
    // enable SPI, set as MASTER, clock to fosc/128 (TODO - can it go faster?)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}

void spi_activate()
{
    PORTB &= ~(1 << PB7);
}

void spi_deactivate()
{
    PORTB |= (1 << PB7);
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