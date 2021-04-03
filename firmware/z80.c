#include "z80.h"

#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "io.h"

#define WAIT     _delay_us(1)

void z80_init()
{
    set_RST(0);
    set_BUSREQ(1);
    set_CLK(0);
    set_INT(1);
}

void z80_powerup()
{
    set_RST(0);
    for (int i = 0; i < 50; ++i)
        z80_cycle();
    set_RST(1);
}

void z80_cycle()
{
    set_CLK(1);
    set_CLK(0);
}

void z80_iorq()
{
    printf_P(PSTR("IORQ!\n"));
}

void z80_set_speed(Z80_Speed speed)
{
	TCCR1B = 0x18; // 0001 1000, Disable Timer Clock 
	TCCR1A = 0xA2; // 1010 0010

	switch (speed) {
		case T_STOPPED:
			TCCR1B = 0;
            return;
        case T_10KHZ:
            ICR1 = 1600-1;
            break;
        case T_1MHZ:
            ICR1 = 16-1;
            break;
        case T_4MHZ:
            ICR1 = 4-1;
            break;
    }

	OCR1A = (int) (ICR1 * 0.50);
	OCR1B = (int) (ICR1 * 0.50);
	TCNT1=0x0;

	TCCR1B |= 1; // Prescale=1, Enable Timer Clock
}
