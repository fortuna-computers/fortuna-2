#include <util/delay.h>

#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "io.h"
#include "serial.h"
#include "z80.h"
#include "ram.h"
#include "tests.h"

int main()
{
    _delay_ms(100);
    serial_init();
    io_init();
    z80_init();
    ram_init();

    printf_P(PSTR("\e[1;1H\e[2JMini-Z80 initialized.\n"));

#ifdef RUN_TESTS
    run_tests();
#endif

    extern unsigned int z80_boot_rom_len;
    extern unsigned char z80_boot_rom[];
    ram_initialize_boot_sector(z80_boot_rom, z80_boot_rom_len);

    z80_powerup();
    z80_set_speed(T_10KHZ);

    for (;;);
}
