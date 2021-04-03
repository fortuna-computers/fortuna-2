#ifndef IO_H_
#define IO_H_

#include <stdbool.h>
#include <stdint.h>

void io_init();

#define ADDR_PORT PORTA
#define ADDR_PIN  PINA
#define ADDR_DDR  DDRA
#define DATA_PORT PORTC
#define DATA_PIN  PINC 
#define DATA_DDR  DDRC

// ports only written by the microcontroller (set_XXX)
#define OUTPUT_PORTS    \
    P(RST,    B, 0)     \
    P(BUSREQ, B, 1)     \
    P(CLK,    B, 2)     \
    P(SDCS,   B, 4)     \
    P(MOSI,   B, 5)     \
    P(SCK,    B, 7)     \
    P(INT,    D, 2)     \
    P(MREQ,   D, 5)     \
    P(WR,     D, 6)     \
    P(RD,     D, 7)

// ports only read by the microcontroller (get_XXX)
#define INPUT_PORTS     \
    P(MISO,    B, 6)    \
    P(IORQ,    D, 3)    \
    P(BUSACK,  D, 4)

// port both read and written by the microcontroller
#define IO_PORTS        \
    P(A8,     B, 3)
    
#define P(name, port, pin)             \
    bool get_last_ ## name();          \
    void set_ ## name(uint8_t v);      \
    void set_ ## name ## _as_output(); \
    void set_ ## name ## _as_high_impedance();
IO_PORTS
OUTPUT_PORTS
#undef P

#define P(name, port, pin)            \
    bool get_ ## name();              \
    void set_ ## name ## _as_input(); \
    void set_ ## name ## _as_pullup();
IO_PORTS
INPUT_PORTS
#undef P

void     set_addr_as_output();
void     set_addr_as_input();
uint16_t get_addr();
void     set_addr(uint16_t addr);

void    set_data_as_output();
void    set_data_as_input();
uint8_t get_data();
void    set_data(uint8_t data);

#endif

// vim:ts=4:sts=4:sw=4:expandtab
