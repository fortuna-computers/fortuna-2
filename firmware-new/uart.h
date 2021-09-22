#ifndef UART_H_
#define UART_H_

#include <avr/pgmspace.h>
#include <stdint.h>

void uart_init();

void uart_putchar(char c);
void uart_puthex(uint8_t v);
void uart_putstr(PGM_P s);

char uart_getchar();

#endif
