// The RAM IC is KM62256CLP-7L (70 ns)
// Each MCU cycle is 60ns

#include "ram.h"
#include "uart.h"

#include <stdbool.h>
#include <avr/cpufunc.h>
#include <avr/io.h>
#include <util/delay.h>

extern volatile uint8_t buffer[512];

#define set_MREQ()    PORTB |= (1 << PB2)
#define clear_MREQ()  PORTB &= ~(1 << PB2)
#define set_WR()      PORTD |= (1 << PD6)
#define clear_WR()    PORTD &= ~(1 << PD6)
#define set_RD()      PORTD |= (1 << PD7)
#define clear_RD()    PORTD &= ~(1 << PD7)
#define set_A8()      PORTB |= (1 << PB3)
#define clear_A8()    PORTB &= ~(1 << PB3)
#define set_ADDR(n)   PORTA = (n)
#define set_DATA(n)   PORTC = (n)
#define get_DATA()    PINC

static void ram_bus_takeover(bool for_writing)
{
    DDRA = 0xff;                      // address low
    DDRB |= (1 << PB3) | (1 << PB2);  // A8 and MREQ
    DDRD |= (1 << PD6) | (1 << PD7);  // WR and RD
    set_MREQ();
    set_WR();
    set_RD();
    
    if (for_writing) {
        DDRC = 0xff; // data
    } else {
        DDRC = 0x0;
        set_DATA(0);
    }
}

static void ram_bus_release()
{
    DDRC = 0x0;                           // data
    DDRA = 0x0;                           // address low
    DDRB &= ~((1 << PB3) | (1 << PB2));   // A8 and MREQ
    DDRD &= ~((1 << PD6) | (1 << PD7));   // WR and RD
    
    clear_MREQ();
    clear_WR();
    clear_RD();
    clear_A8();
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
        uint8_t data;
        do {
            // write data
            set_DATA(buffer[addr]);
            set_ADDR(addr & 0xff);
            if (addr >= 0x100) set_A8(); else clear_A8();
            clear_MREQ();
            clear_WR();
            set_WR();
            set_MREQ();
            
            // verify  (TODO - remove this?)
            DDRC = 0x0;
            PORTC = 0;
            clear_MREQ();
            clear_RD();
            data = get_DATA();
            set_RD();
            set_MREQ();
            DDRC = 0xff;
    
        } while (data != buffer[addr]);
    }
    
    ram_bus_release();
}

void ram_read_buffer(uint16_t until)
{
    
    for (uint16_t addr = 0; addr < until; ++addr) {
        ram_bus_takeover(false);
        set_ADDR(addr & 0xff);
        if (addr >= 0x100) set_A8(); else clear_A8();
        clear_MREQ();
        clear_RD();
        buffer[addr] = get_DATA();
        set_MREQ();
        set_RD();
        ram_bus_release();
    }
    
}

void ram_write_byte(uint16_t addr, uint8_t data)
{
    ram_bus_takeover(true);

    set_DATA(data);
    set_ADDR(addr & 0xff);
    if (addr >= 0x100) set_A8(); else clear_A8();
    clear_MREQ();
    clear_WR();
    set_WR();
    set_MREQ();

    ram_bus_release();
}

uint8_t ram_read_byte(uint16_t addr)
{
    ram_bus_takeover(false);
    
    set_ADDR(addr & 0xff);
    if (addr >= 0x100) set_A8(); else clear_A8();
    clear_MREQ();
    clear_RD();
    uint8_t data = get_DATA();
    set_MREQ();
    set_RD();
    
    ram_bus_release();

    return data;
}

void ram_dump(uint16_t until)
{
    ram_read_buffer(until);
    
    for (uint16_t i = 0; i < until; ++i)
        uart_puthex(buffer[i]);
    uart_putenter();
}
