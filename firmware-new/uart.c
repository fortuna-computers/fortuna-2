#include "uart.h"

#include <avr/io.h>
#include <util/setbaud.h>

void uart_init()
{
    // set speed
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;

    // set config
    UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);   // Async-mode 
    UCSRB = (1<<RXEN) | (1<<TXEN);     // Enable Receiver and Transmitter
}

void uart_putchar(char c)
{
    while (!( UCSRA & (1<<UDRE))); // Wait for empty transmit buffer
    UDR = c;
}

static void uart_puthex_v(uint8_t v)
{
    if (v <= 9)
        uart_putchar(v + '0');
    else
        uart_putchar(v + 'A' - 10);
}

void uart_puthex(uint8_t v)
{
    uart_puthex_v(v >> 4);
    uart_puthex_v(v & 0xf);
    uart_putchar(' ');
}

void uart_putstr(PGM_P s)
{
    while (1) {
        char c = pgm_read_byte(s);
        if (c == 0)
            return;
        uart_putchar(c);
        ++s;
    }
}

char uart_getchar()
{
    while (!( UCSRA & (1<<RXC)));  // wait for empty receive buffer
    return UDR;
}
