#ifndef Z80_H_
#define Z80_H_

#include <stdint.h>

void z80_init(uint16_t speed_khz);
void z80_powerup();
void z80_powerdown();

void z80_run();
void z80_pause();

void z80_single_step();

#endif