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

#ifdef DEBUG
    run_tests();
#endif

    printf_P(PSTR("? "));
    while (1) {
        putchar(getchar());
        
        // printf("%02X ", getchar());
    }

    for (;;);
}
