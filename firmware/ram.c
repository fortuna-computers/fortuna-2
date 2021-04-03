#include "ram.h"

#include <stddef.h>
#include <stdio.h>

#include <avr/pgmspace.h>
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

uint16_t ram_buffer_checksum()
{
    uint16_t sum1 = 0, sum2 = 0;
    for (size_t i = 0; i < 512; ++i) {
        sum1 = (sum1 + buffer[i]) % 0xff;
        sum2 = (sum2 + sum1) % 0xff;
    }
    return (sum2 << 8) | sum1;
}

void ram_initialize_boot_sector(uint8_t* data, uint16_t size)
{
    for (size_t i = 0; i < 512; ++i)
        buffer[i] = (i < size) ? data[i] : 0;
    if (!ram_write_buffer())
        printf_P(PSTR("Memory error initializing boot sector.\n"));
    uint16_t chk1 = ram_buffer_checksum();
    ram_read_buffer();
    uint16_t chk2 = ram_buffer_checksum();
    printf_P(PSTR("Boot sector loaded with %d bytes: (%04X) (%04X).\n"), size, chk1, chk2);
    if (chk1 != chk2)
        printf_P(PSTR("Memory error initializing boot sector: invalid checksum.\n"));
}
