#include "z80.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#define set_RST()   PORTB |= (1 << PB0)
#define clear_RST() PORTB &= ~(1 << PB0)
#define set_CLK()   PORTD |= (1 << PD5)
#define clear_CLK() PORTD &= ~(1 << PD5)

volatile uint16_t z80_speed_khz = 0;

void z80_init(uint16_t speed_khz)
{
    z80_speed_khz = speed_khz;
    
    DDRB |= (1 << PB0);     // RST
    DDRD |= (1 << PD5);     // CLK
    
    z80_powerdown();
}

void z80_powerdown()
{
    // reset (initialize as low)
    clear_RST();
    
    // run 50 cycles
    for (uint8_t i = 0; i < 50; ++i) {
        set_CLK();
        clear_CLK();
    }
}

void z80_powerup()
{
    clear_RST();
    z80_run();
}

void z80_run()
{
    // clock output on OC1A (PD5)
    
    OCR1A = F_CPU / (2UL * (unsigned long) z80_speed_khz * 1000UL) - 1UL;  // calculate speed
    TCCR1A = (1 << COM1A0);                  // Toggle OC1A/OC1B on compare match
    TCCR1B = (1 << WGM12) | (1 << CS10);     // CTC mode, top OCR1A, no prescaling (clk/1)

    // CTC mode 4
    // F_TIMER = F_CPU / (2 * PRESCALER * (1 + OCR1A))
    // https://ee-diary.blogspot.com/2021/08/arduino-ctc-mode-programming-with.html
    // https://ee-diary.blogspot.com/p/atmega32-timer1-online-calculator.html (CTC mode)
}

void z80_pause()
{
    TCCR1A = 0;
}
