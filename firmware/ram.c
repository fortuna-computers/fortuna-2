#include "ram.h"

#include <stddef.h>
#include <util/delay.h>

#include "io.h"

#define WAIT     _delay_us(1)

uint8_t buffer[512] = {0};

void ram_init()
{
    set_MREQ(1);
    set_WR(1);
    set_RD(1);
    set_data_as_input();
    set_addr_as_input();
}

uint8_t ram_read_byte(uint16_t addr)
{
    set_addr_as_output();
    set_addr(addr);
    set_MREQ(0);
    set_RD(0);
    WAIT; WAIT; WAIT;
    uint8_t data = get_data();
    set_RD(1);
    set_MREQ(1);
    set_addr_as_input();
    return data;
}

void ram_write_byte(uint16_t addr, uint8_t byte)
{
    set_addr_as_output();
    set_addr(addr);
    set_data_as_output();
    set_data(byte);
    set_MREQ(0);
    set_WR(0);
    WAIT; WAIT; WAIT;
    set_WR(1);
    set_MREQ(1);
    set_data(0);
    set_addr_as_input();
    set_data_as_input();
}

bool ram_write_buffer()
{
    for (size_t i = 0; i < 512; ++i) {
        for (size_t j = 0; j < 16; ++j) {
            ram_write_byte(i, buffer[i]);
            uint8_t data = ram_read_byte(i);
            if (data == buffer[i])
               goto next;
            WAIT; WAIT; WAIT;
        }
        return false;
next:;
    }
    return true;
}

void ram_read_buffer()
{
    for (size_t i = 0; i < 512; ++i)
        buffer[i] = ram_read_byte(i);
}
