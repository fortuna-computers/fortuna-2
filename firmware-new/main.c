#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

#include "io.h"
#include "ram.h"
#include "random.h"
#include "uart.h"
#include "z80.h"
#include "post.h"
#include "spi.h"

#define SPEED_KHZ 10

volatile uint8_t buffer[512] = { 0 };

static void initialize_fortuna();

__attribute__((unused)) static void check_mcucsr();

int main()
{
    initialize_fortuna();
    post_run();
    
    // TODO - load EEPROM into RAM
    
    z80_powerup();
    z80_run();
    
    for(;;);
}

static void initialize_fortuna()
{
    // initialize serial
    uart_init();
    uart_putstr(PSTR("\033[2J\033[H"));  // clear screen
    
    // check last status
    // check_mcucsr();
    
    // initialize devices
    rnd_init();
    ram_init();
    z80_init(SPEED_KHZ);    // the Z80 is now on a reset state (addr/data lines in high impedance)
    spi_init();
    io_init();
    _delay_ms(50);
}

ISR(BADISR_vect)   // catch all interrupts
{
    uart_putchar('!');
    uart_putchar('!');
    uart_putchar('!');
    for(;;);
}

__attribute__((unused)) static void check_mcucsr()
{
    if (MCUCSR == 0)
        return;

    uart_putchar('!');

    if (bit_is_set(MCUCSR, PORF))
        uart_putchar('#');
    if (bit_is_set(MCUCSR, EXTRF))
        uart_putchar('%');
    if (bit_is_set(MCUCSR, BORF))
        uart_putchar('~');
    if (bit_is_set(MCUCSR, WDRF))
        uart_putchar('W');
    if (bit_is_set(MCUCSR, JTRF))
        uart_putchar('J');

    uart_putchar(' ');
    MCUCSR = 0;
}
