#include "random.h"

#include <avr/eeprom.h>

#define SEED_ADDR (uint8_t*) 511

volatile uint8_t next = 0;

uint8_t rnd_init()
{
    uint8_t seed = eeprom_read_byte(SEED_ADDR);
    next = seed;
    eeprom_write_byte(SEED_ADDR, next);
    return seed;
}

uint8_t rnd_next()
{
    next = (uint8_t) (((uint32_t) next) * 89 + 207);
    return next;
}

uint8_t rnd_next_from(uint8_t current)
{
    return (uint8_t) (((uint32_t) current) * 89 + 207);
}