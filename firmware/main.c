#include <util/delay.h>

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "io.h"
#include "serial.h"
#include "z80.h"
#include "ram.h"
#include "step.h"
#include "tests.h"

static const Z80_Speed z80_speed = T_100HZ;

int main()
{
    // microcontroller initialization
    _delay_ms(100);
    serial_init();
    io_init();
    z80_init();
    ram_init();

#ifdef RUN_TESTS
    run_tests();
#endif

    // computer initialization
    printf_P(PSTR("\e[1;1H\e[2JMini-Z80 initialized.\n"));

    extern unsigned int z80_boot_rom_len;
    extern unsigned char z80_boot_rom[];
    ram_initialize_boot_sector(z80_boot_rom, z80_boot_rom_len);

    z80_powerup();

    // enable interrupts
    GICR = (1 << INT1);     // enable interrupt on INT1 (IORQ)  
    MCUCR = (1 << ISC11);   //   on falling edge
    sei();

    // run
#ifndef STEP
    z80_set_speed(z80_speed);
    for (;;);
#else
    step_init();
    for (;;) {
        step();
    }
#endif
}


ISR(INT1_vect)   // execute on IORQ
{
    TCCR1B = 0;                // stop Z80 cycles
    z80_iorq();
    z80_set_speed(z80_speed);  // reactivate Z80 cycles
}
