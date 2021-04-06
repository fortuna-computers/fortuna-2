#include "z80.h"

#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "io.h"
#include "ram.h"
#include "sdcard.h"

#define WAIT      _delay_us(1)

extern uint16_t   last_pressed_key;

volatile int      next_interrupt = -1;
volatile uint32_t sdcard_block = 0;

void z80_init()
{
    set_RST(0);
    set_BUSREQ(1);
    set_CLK(0);
    set_INT(1);
    set_SCK_WAIT(1);
    z80_cycle();
    _delay_us(100);
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

void z80_release_bus()
{
    z80_set_speed(T_STOPPED);
    if (get_BUSACK() == 0)
        return;
#ifdef DEBUG
    printf_P(PSTR("Requesting bus: "));
#endif
    set_BUSREQ(0);
    while (get_BUSACK() != 0) {
#ifdef DEBUG
        // putchar('.');
#endif
        z80_cycle();
    }
#ifdef DEBUG
    printf_P(PSTR("bus obtained!\n"));
#endif
    set_MREQ_as_output();
    set_WR_as_output();
    set_RD_as_output();
}

void z80_request_bus()
{
    set_BUSREQ(1);
    set_MREQ_as_high_impedance();
    set_WR_as_high_impedance();
    set_RD_as_high_impedance();
}

static void sd_read_to_ram()
{
#ifdef DEBUG
    printf_P(PSTR("Reading block %02X from SDCard to RAM.\n"), sdcard_block);
#endif
    z80_release_bus();
    bool ok = sdcard_read_page(sdcard_block);
#ifdef DEBUG
    for (int i = 0; i < 512; ++i)
        printf_P(PSTR("%02X"), buffer[i]);
    putchar('\n');
#endif
    ram_write_buffer();
    z80_request_bus();
#ifdef DEBUG
    if (ok) 
        printf_P(PSTR("Reading done.\n"));
    else
        printf_P(PSTR("Reading failed.\n"));
#endif
}

static void sd_write_from_ram()
{
#ifdef DEBUG
    printf_P(PSTR("Writing from RAM to SDCard block %02X.\n"), sdcard_block);
#endif
    z80_release_bus();
    ram_read_buffer();
#ifdef DEBUG
    for (int i = 0; i < 512; ++i)
        printf_P(PSTR("%02X"), buffer[i]);
    putchar('\n');
#endif
    bool ok = sdcard_write_page(sdcard_block);
    z80_request_bus();
#ifdef DEBUG
    if (ok) 
        printf_P(PSTR("Writing done.\n"));
    else
        printf_P(PSTR("Writing failed.\n"));
#endif
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
        case I_SD_B0:
            sdcard_block |= data;
            break;
        case I_SD_B1:
            sdcard_block |= ((uint32_t) data << 8);
            break;
        case I_SD_B2:
            sdcard_block |= ((uint32_t) data << 16);
            break;
        case I_SD_B3:
            sdcard_block |= ((uint32_t) data << 24);
            break;
        case I_SD_ACTION:
            if (data & 1)
                sd_write_from_ram();
            else
                sd_read_to_ram();
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
    if (speed == T_STOPPED) {
        TCCR1A = 0;
        TCCR1B = 0;
        ICR1 = 0;
        OCR1A = 0;
        OCR1B = 0;
        TCNT1 = 0;
        return;
    }

	TCCR1B = 0x18; // 0001 1000, Disable Timer Clock 
	TCCR1A = 0xA2; // 1010 0010

	switch (speed) {
        case T_STOPPED:
            break;
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
