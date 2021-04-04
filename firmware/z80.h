#ifndef Z80_H_
#define Z80_H_

#include <stdbool.h>

typedef enum {
    I_TERMINAL = 0x1,
    I_KEYBOARD = 0x2,
} IORQ;

void z80_init();
void z80_powerup();
void z80_cycle();
void z80_iorq(bool wr, bool rd);

typedef enum { T_STOPPED, T_100HZ, T_10KHZ, T_100KHZ, T_450KHZ, T_1MHZ, T_1_5MHZ, T_2MHZ, T_4MHZ } Z80_Speed;
void z80_set_speed(Z80_Speed speed);

#endif
