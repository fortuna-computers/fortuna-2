#ifndef Z80_H_
#define Z80_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    I_TERMINAL      = 0x1,
    I_SD_B0         = 0x2,
    I_SD_B1         = 0x3,
    I_SD_B2         = 0x4,
    I_SD_B3         = 0x5,
    I_SD_ACTION     = 0x6,  // 0 = read, 1 = write
} IORQ;

#define KEYBOARD_DEVICE 0x0   // when a keyboard interrupt happens, this will be put on the data bus

void z80_init();
void z80_powerup();
void z80_release_bus();
void z80_request_bus();
void z80_cycle();
void z80_iorq(bool wr, bool rd);
void z80_interrupt(uint8_t code);

typedef enum { 
    T_STOPPED,
    T_100HZ,
    T_10KHZ, T_100KHZ, T_550KHZ, T_800KHZ,
    T_1MHZ, T_1_5MHZ, T_2MHZ, T_4MHZ
} Z80_Speed;
void z80_set_speed(Z80_Speed speed);

#endif
