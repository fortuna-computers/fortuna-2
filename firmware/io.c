#include "io.h"

#include <avr/io.h>

//
// setters
//

#define P(name, port, pin)                                 \
    static bool p_ ## name = false;                        \
    bool get_last_ ## name() {                             \
        return p_ ## name;                                 \
    }                                                      \
    void set_ ## name(uint8_t v)                           \
    {                                                      \
        if (v)                                             \
            PORT ## port |= _BV(PORT ## port ## pin);      \
        else                                               \
            PORT ## port &= ~_BV(PORT ## port ## pin);     \
        p_ ## name = v;                                    \
    }                                                      \
    void set_ ## name ## _as_output()                      \
    {                                                      \
        DDR ## port |= _BV(PIN ## port ## pin);            \
    }                                                      \
    void set_ ## name ## _as_high_impedance()              \
    {                                                      \
        DDR ## port &= ~_BV(PIN ## port ## pin);           \
        PORT ## port &= ~_BV(PORT ## port ## pin);         \
    }
IO_PORTS
OUTPUT_PORTS
#undef P

// 
// getters
//

#define P(name, port, pin)                                 \
    bool get_ ## name()                                    \
    {                                                      \
        return PIN ## port & _BV(PIN ## port ## pin);      \
    }                                                      \
    void set_ ## name ## _as_input()                       \
    {                                                      \
        DDR ## port &= ~_BV(PIN ## port ## pin);           \
        PORT ## port |= _BV(PORT ## port ## pin);          \
    }                                                      \
    void set_ ## name ## _as_pullup()                      \
    {                                                      \
        DDR ## port &= ~_BV(PIN ## port ## pin);           \
        PORT ## port &= ~_BV(PORT ## port ## pin);         \
    }
INPUT_PORTS
IO_PORTS
#undef P

//
// initialize ports
//

void io_init()
{
#define P(name, port, pin) set_ ## name ## _as_output();
    OUTPUT_PORTS
#undef P
#define P(name, port, pin) set_ ## name ## _as_pullup();
    INPUT_PORTS
    IO_PORTS
#undef P
}

void set_addr_as_output()
{
    ADDR_DDR = 0xff;
    set_A8_as_output();
}

void set_addr_as_input()
{
    ADDR_DDR = 0x0;
    set_A8_as_input();
}

static unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

uint16_t get_addr()
{
    uint8_t byte = ADDR_PIN;
    uint16_t word = reverse(byte);
    if (get_A8())
        word |= (1 << 8);
    return word;
}

void set_addr(uint16_t addr)
{
    uint8_t byte = reverse((uint8_t) addr);
    ADDR_PORT = byte;
    if (addr & (1 << 8))
        set_A8(1);
    else
        set_A8(0);
}

void set_data_as_output()
{
    DATA_DDR = 0xff;
}

void set_data_as_input()
{
    DATA_DDR = 0x0;
}

uint8_t get_data()
{
    return DATA_PIN;
}

void set_data(uint8_t data)
{
    DATA_PORT = data;
}

// vim:ts=4:sts=4:sw=4:expandtab
