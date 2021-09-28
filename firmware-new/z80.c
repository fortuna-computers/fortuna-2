#include "z80.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#define set_RST()   PORTB |= (1 << PB0)
#define clear_RST() PORTB &= ~(1 << PB0)
#define set_CLK()   PORTD |= (1 << PD5)
#define clear_CLK() PORTD &= ~(1 << PD5)

uint16_t z80_speed_khz = 0;

void z80_init(uint16_t speed_khz)
{
    z80_speed_khz = speed_khz;
    
    DDRB |= (1 << PB0);     // RST
    DDRD |= (1 << PD5);     // CLK
    
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
    
    TCCR1A = (1 << WGM11)     // Fast PWM, top = ICR1
           | (1 << COM1B1)    // Clear OC1A/OC1B on compare match (Set output to low level)
           | (1 << COM1A1);
    TCCR1B = (1 << WGM12)
           | (1 << WGM13);
           
    ICR1 = (F_CPU / z80_speed_khz) - 1;
    
    OCR1A = (int) (ICR1 * 0.50);  // 50% duty cycle
    OCR1B = (int) (ICR1 * 0.50);
    
    TCCR1B = (1 << CS10); // Prescale=1, Enable Timer Clock
    
    TCNT1 = 0x0;
}

void z80_pause()
{
    cli();
    TCCR1A = 0;
    TCCR1B = 0;
    ICR1 = 0;
    OCR1A = 0;
    OCR1B = 0;
    TCNT1 = 0;
    sei();
}