#include <util/delay.h>

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "io.h"
#include "serial.h"
#include "z80.h"
#include "ram.h"
#include "sdcard.h"
#include "spi.h"
#include "step.h"
#include "tests.h"

static const Z80_Speed z80_speed = T_550KHZ;

volatile uint16_t last_pressed_key = 0x0;

int main()
{
    _delay_ms(100);

    // microcontroller initialization
    serial_init();
    io_init();
    z80_init();
    ram_init();
    spi_init();

#ifdef RUN_TESTS
    run_tests();
#endif

    // computer initialization
    printf_P(PSTR("\e[1;1H\e[2JMini-Z80 initialized.\n"));

    // initialize SD card
    bool ok = sdcard_initialize();
    printf_P(PSTR("SDCard %sinitialized. %02X %02X\n"), 
            ok ? "" : "not ",
            sdcard_last_stage(), sdcard_last_response());

    // boot
    extern unsigned int z80_boot_rom_len;
    extern unsigned char z80_boot_rom[];
    z80_powerup();
    z80_release_bus();
    ram_initialize_boot_sector(z80_boot_rom, z80_boot_rom_len);
    z80_request_bus();

    // enable interrupt for IORQ
    GICR = (1 << INT1);     // enable interrupt on INT1 (IORQ)  
    MCUCR = (1 << ISC11);   //   on falling edge
    sei();

    z80_init();
    z80_powerup();

#ifndef STEP
    // enable interrupt for keypress
    UCSRB |= (1 << RXEN) | (1 << RXCIE);

    // run
    z80_set_speed(z80_speed);
    for (;;);
#else
    step_init();
    for (;;) {
        step();
    }
#endif
}


ISR(INT1_vect)   // interrupt: execute on IORQ
{
    TCCR1B = 0;                // stop Z80 cycles
    // set_SCK_WAIT(0);
    bool rd = get_RD();
    bool wr = get_WR();
    cli();

    z80_iorq(wr, rd);
    
    sei();
    // set_SCK_WAIT(1);
    z80_set_speed(z80_speed);  // reactivate Z80 cycles
}

ISR(USART_RXC_vect)   // interrupt: execute on keypress
{
    last_pressed_key = serial_recv();
    z80_interrupt(0xcf);
}
