#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdint.h>

uint8_t rnd_seed();
uint8_t rnd_next(uint8_t current);

#endif
