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
    set_SCK_WAIT(1);
}

void z80_powerup()
{
    set_RST(0);
    for (int i = 0; i < 50; ++i)
        z80_cycle();

    set_MREQ_as_high_impedance();
    set_WR_as_high_impedance();
    set_RD_as_high_impedance();
    set_INT(1);
    set_BUSREQ(1);
    set_SCK_WAIT(1);

    set_RST(1);
}

void z80_cycle()
{
    set_CLK(1);
    set_CLK(0);
}

static void z80_out(uint8_t port, uint8_t data)
{
    switch (port) {
        case I_TERMINAL:
            putchar(data);
            break;
    }
}

void z80_iorq(bool wr, bool rd)
{
    uint8_t port = get_addr();
#ifdef DEBUG
    printf_P(PSTR("IORQ port 0x%02X! (WR: %d)\n"), port, wr);
#endif
    if (wr == 0) {
        uint8_t data = get_data();
#ifdef DEBUG
        printf_P(PSTR("OUTPUT request in port 0x%02X with data 0x%02X.\n"), port, data);
#endif
        z80_out(port, data);
    }
}

void z80_set_speed(Z80_Speed speed)
{
	TCCR1B = 0x18; // 0001 1000, Disable Timer Clock 
	TCCR1A = 0xA2; // 1010 0010

	switch (speed) {
		case T_STOPPED:
			TCCR1B = 0;
            return;
        case T_100HZ:
            ICR1 = 20625-1;
            break;
        case T_10KHZ:
            ICR1 = 1600-1;
            break;
        case T_100KHZ:
            ICR1 = 165-1;
            break;
        case T_450KHZ:
            ICR1 = 37-1;
            break;
        case T_1MHZ:
            ICR1 = 21-1;
            break;
        case T_1_5MHZ:
            ICR1 = 12-1;
            break;
        case T_2MHZ:
            ICR1 = 8-1;
            break;
        case T_4MHZ:
            ICR1 = 4-1;
            break;
    }

	OCR1A = (int) (ICR1 * 0.50);
	OCR1B = (int) (ICR1 * 0.50);
	TCNT1=0x0;

    if (speed == T_100HZ)
        TCCR1B |= 2; // Prescale=8, Enable Timer Clock
    else
        TCCR1B |= 1; // Prescale=1, Enable Timer Clock
}
