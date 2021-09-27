// The RAM IC is KM62256CLP-7L (70 ns)
// Each MCU cycle is 60ns

#include "ram.h"
#include "uart.h"

#include <stdbool.h>
#include <avr/cpufunc.h>
#include <avr/io.h>
#include <util/delay.h>

extern volatile uint8_t buffer[512];

#define set_MREQ(n)   PORTB |= (1 << PB2)
#define clear_MREQ(n) PORTB &= ~(1 << PB2)
#define set_WR(n)     PORTD |= (1 << PD6)
#define clear_WR(n)   PORTD &= ~(1 << PD6)
#define set_RD(n)     PORTD |= (1 << PD7)
#define clear_RD(n)   PORTD &= ~(1 << PD7)
#define set_A8(n)     PORTB |= (1 << PB3)
#define clear_A8(n)   PORTB &= ~(1 << PB3)
#define set_ADDR(n)   PORTA = n
#define set_DATA(n)   PORTC = n
#define get_DATA()    PINC

static void ram_bus_takeover(bool for_writing)
{
    DDRA = 0xff;                         // address low
    DDRB |= (1 << PB3) | (1 << PB2);     // A8 and MREQ
    DDRD &= ~((1 << PD6) | (1 << PD7));  // WR and RD
    set_MREQ(1);
    set_WR(1);
    set_RD(1);
    
    if (for_writing)
        DDRC = 0x0; // data
    
    _delay_us(20);
}

static void ram_bus_release()
{
    DDRC = 0x0;                           // data
    DDRA = 0x0;                           // address low
    DDRB &= ~((1 << PB3) | (1 << PB2));   // A8 and MREQ
    DDRD &= ~((1 << PD6) | (1 << PD7));   // WR and RD
    
    set_MREQ(0);
    set_WR(0);
    set_RD(0);
    set_A8(0);
    set_ADDR(0);
    set_DATA(0);
}

void ram_init()
{
    ram_bus_release();
}

void ram_write_buffer(uint16_t until)
{
    ram_bus_takeover(true);
    
    for (uint16_t addr = 0; addr < until; ++addr) {
        set_DATA(buffer[addr]);
        set_ADDR(addr & 0xff);
        set_A8((addr >> 8) & 1);
        set_MREQ(0);
        set_WR(0);
        _NOP(); _NOP();
        set_WR(1);
        set_MREQ(1);
        _NOP(); _NOP();
    }
    
    ram_bus_release();
}

void ram_read_buffer(uint16_t until)
{
    ram_bus_takeover(false);
    
    for (uint16_t addr = 0; addr < until; ++addr) {
        set_ADDR(addr & 0xff);
        set_A8((addr >> 8) & 1);
        set_MREQ(0);
        set_RD(0);
        _NOP(); _NOP();
        buffer[addr] = get_DATA();
        set_RD(1);
        set_MREQ(1);
        _NOP(); _NOP();
    }
    
    ram_bus_release();
}

void ram_dump(uint16_t until)
{
    ram_read_buffer(until);
    
    for (uint16_t i = 0; i < until; ++i)
        uart_puthex(buffer[i]);
    uart_putchar('\n');
}