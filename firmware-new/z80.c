#include "z80.h"

#include <avr/io.h>

#define set_RST()   PORTB |= (1 << PB0)
#define clear_RST() PORTB &= ~(1 << PB0)
#define set_CLK()   PORTD |= (1 << PD5)
#define clear_CLK() PORTD &= ~(1 << PD5)

void z80_init()
{
    DDRB |= (1 << PB0);     // RST
    DDRD |= (1 << PD5);     // CLK
    
    // reset (initialize as low)
    clear_RST();
    
    // run 50 cycles
    for (uint8_t i = 0; i < 50; ++i) {
        set_CLK();
        clear_CLK();
    }
}