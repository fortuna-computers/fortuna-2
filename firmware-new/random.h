#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdint.h>

uint8_t rnd_init();
uint8_t rnd_next();
uint8_t rnd_next_from(uint8_t current);

#endif
