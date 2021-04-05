#include "z80.h"

#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "io.h"

#define WAIT     _delay_us(1)

extern uint16_t last_pressed_key;

volatile int next_interrupt = -1;

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

static void z80_out(uint8_t port)
{
    uint8_t data = get_data();
#ifdef DEBUG
    printf_P(PSTR("OUTPUT request in port %02X with data %02X.\n"), port, data);
#endif
    switch (port) {
        case I_TERMINAL:
            putchar(data);
            break;
    }
}

static void z80_in(uint8_t port)
{
    uint8_t data = 0;
    switch (port) {
        case I_TERMINAL:
            data = last_pressed_key;
            last_pressed_key = 0;
            break;
        default:
            return;
    }

#ifdef DEBUG
    printf_P(PSTR("INPUT request from port %02X (data: %02X).\n"), port, data);
#endif
    set_data_as_output();
    set_data(data);
    z80_cycle();
    set_data_as_input();
}

static void z80_iorq_int()
{
#ifdef DEBUG
    printf_P(PSTR("INTERRUPT request (data: %02X).\n"), next_interrupt);
#endif
    if (next_interrupt >= 0) {
        set_data_as_output();
        set_data(next_interrupt);
        // while (get_IORQ() == 0)
            z80_cycle();
        set_data_as_input();
        next_interrupt = -1;
    }
    set_INT(1);
#ifdef DEBUG
    printf_P(PSTR("Interrupt process completed.\n"));
#endif
}

void z80_iorq(bool wr, bool rd)
{
    uint8_t port = get_addr();
#ifdef DEBUG
    printf_P(PSTR("IORQ port %02X! (WR: %d, RD: %d)\n"), port, wr, rd);
#endif
    if (wr == 0)
        z80_out(port);
    else if (rd == 0)
        z80_in(port);
    else   // interrupt
        z80_iorq_int();
}

void z80_interrupt(uint8_t code)
{
#ifdef DEBUG
    printf_P(PSTR("Interrupt!\n"));
#endif
    next_interrupt = code;
    set_INT(0);
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
        case T_550KHZ:
            ICR1 = 30-1;
            break;
        case T_800KHZ:
            ICR1 = 21-1;
            break;
        case T_1MHZ:
            ICR1 = 16-1;
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
