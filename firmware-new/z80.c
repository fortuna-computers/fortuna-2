#include "z80.h"

#include <avr/io.h>

void z80_init()
{
    // reset (initialize as low)
    DDRB |= (1 << PB0);
    PORTB &= ~(1 << PB0);
}