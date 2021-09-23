#include "random.h"

#include <avr/eeprom.h>

#define SEED_ADDR (uint8_t*) 511

uint8_t rnd_seed()
{
    uint8_t seed = eeprom_read_byte(SEED_ADDR);
    uint8_t next = rnd_next(seed);
    eeprom_write_byte(SEED_ADDR, next);
    return seed;
}

uint8_t rnd_next(uint8_t current)
{
    return (uint8_t) (((uint32_t) current) * 89 + 207);
}
