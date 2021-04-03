#ifndef Z80_H_
#define Z80_H_

void z80_init();
void z80_powerup();
void z80_cycle();
void z80_iorq();

typedef enum { T_STOPPED, T_10KHZ, T_1MHZ, T_4MHZ } Z80_Speed;
void z80_set_speed(Z80_Speed speed);

#endif
