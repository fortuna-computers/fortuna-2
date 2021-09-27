// The RAM IC is KM62256CLP-7L

#include "ram.h"

#include <avr/io.h>

extern volatile uint8_t buffer[512];

void ram_init()
{
    DDRA = 0x0;          // address low - currently in high impedance
    DDRC = 0x0;          // data - currently in high impedance
    DDRB &= ~((1 << PB3) | (1 << PB2));   // A8 and MREQ - currently in low impedance
    DDRD &= ~((1 << PD6) | (1 << PD7));   // WR and RD - currently in low impedance
}

void ram_write_buffer(uint16_t until)
{
}

void ram_read_buffer(uint16_t until)
{
}