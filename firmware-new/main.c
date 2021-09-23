#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

#include "ram.h"
#include "random.h"
#include "uart.h"

uint8_t buffer[512] = { 0 };
int     seed;

static void check_mcucsr();

int main()
{
    _delay_ms(50);
    uart_init();

    check_mcucsr();
    seed = rnd_seed();
    ram_init();

    uart_puthex(seed);

    for(;;);
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
