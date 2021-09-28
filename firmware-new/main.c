#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

#include "ram.h"
#include "random.h"
#include "uart.h"
#include "z80.h"
#include "post.h"
#include "spi.h"

volatile uint8_t seed;
volatile uint8_t buffer[512] = { 0 };

static void initialize_fortuna();
static void check_mcucsr();

int main()
{
    initialize_fortuna();
    
    post_run();
    
    for(;;);
}

static void initialize_fortuna()
{
    // initialize serial
    uart_init();
    uart_putstr(PSTR("\033[2J\033[H"));  // clear screen
    
    // check last status
    check_mcucsr();
    seed = rnd_seed();
    uart_puthex(seed);   // TODO - remove
    uart_putenter();     // TODO - remove
    
    // initialize devices
    ram_init();
    z80_init();    // the Z80 is now on a reset state (addr/data lines in high impedance)
    spi_init();
    _delay_ms(50);
}

ISR(BADISR_vect)   // catch all interrupts
{
    uart_putchar('!');
    uart_putchar('!');
    uart_putchar('!');
    for(;;);
}

static void check_mcucsr()
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
